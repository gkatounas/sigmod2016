#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "graph.h"
#include "jsch.h"
#include "grail.h"
#include "bbfs.h"
#include "SCC.h"
#include "WCC.h"

extern pthread_mutex_t mtx, thr_mtx, count_mtx, job_mtx;
extern pthread_cond_t empty_queue, full_queue;
extern JobScheduler *js;
extern uint32_t q, jobs_count, exit_thread;
int thr_id=0;

void initialize_scheduler(){		
	int i, t=1;
	js=malloc(sizeof(JobScheduler));
	
	js->execution_threads=NUM_OF_THREADS;
	initQ(100);						// Initialize queue
	js->tids=malloc(NUM_OF_THREADS*sizeof(pthread_t));
	js->result=malloc(SIZE*sizeof(int));
	js->size=SIZE;
	for(i=0;i<NUM_OF_THREADS;i++){					// Create threads
		t=pthread_create(&(js->tids[i]),NULL,&thread_function,NULL);
		if(t!=0) printf("fail thread\n");
	}
}

void submit_job(void* function, void** args){		// Insert job in queue
	int size=js->q->size;

	if(js->q->count==size){
		size=2*size;
		js->q->job=(Job*) realloc(js->q->job,size*sizeof(Job));
		js->q->size=size;
	}
	js->q->job[js->q->count].function=function;
	js->q->job[js->q->count].args=args;
	js->q->count++;	
}

void execute_all_jobs() {				// Execute all jobs in queue
	if(js->size<js->q->count) {
		js->result=(int*) realloc(js->result,js->q->count*sizeof(int));
		js->size=js->q->count;
	}
	js->count=js->q->count;
	jobs_count=js->q->count;
	pthread_cond_broadcast(&full_queue);		// Broadcast in order to wake up threads and do the jobs
	if (pthread_mutex_unlock(&job_mtx)) printf("pthread_mutex_unlock");	// Unlock mutex for jobs queue
}

void wait_all_tasks_finish() {		// Wait all submitted tasks to finish
	if (pthread_mutex_lock(&job_mtx)) printf("pthread_mutex_lock");		// Lock mutex for jobs queue
	while(jobs_count!=0)
		pthread_cond_wait(&empty_queue, &job_mtx);						// Wait if queue of jobs is not empty
}

OK_SUCCESS destroy_scheduler() {	
	int i, status;
	
	exit_thread=1;
	free(js->q->job);
	free(js->q);
	free(js->tids);
	free(js->result);
	free(js);
	return SUCCESS;
}

void initQ(int qsize){				// Jobs queue initialization
	js->q=malloc(sizeof(Queue));
	js->q->size=qsize;
	js->q->count=0;
	(js->q->job)=malloc(qsize*sizeof(Job));
	js->q->first=0;
}

void* thread_function() {
	int thread_id, (*func)(void**);
	void **arg;
	
	if (pthread_mutex_lock(&thr_mtx)) printf("pthread_mutex_lock");		// Lock mutex for thread_id
	thread_id=thr_id++;
	if (pthread_mutex_unlock(&thr_mtx)) printf("pthread_mutex_unlock");	// Unlock mutex for thread_id
	while(1) {
		if (pthread_mutex_lock(&job_mtx)) printf("pthread_mutex_lock");		// Lock mutex for jobs queue
		while((js->q->count==0) && (exit_thread==0))
			pthread_cond_wait(&full_queue, &job_mtx);				// Wait if queue of jobs is empty
		if(exit_thread==1)	pthread_exit(NULL);
		func=js->q->job[js->q->first].function;					// Get the function that should be executed

		arg=js->q->job[js->q->first].args;							// Get the arguments of the function
		js->q->first++;
		js->q->count--;													// Remove job from queue

		arg[0]=&thread_id;
		if (pthread_mutex_unlock(&job_mtx)) printf("pthread_mutex_unlock");	// Unlock mutex for jobs queue
		(*func)(arg);														// Execute the function
	}
}

int dynamic_query(void **args) {					// This function is executed by a thread in case of static graph 
	int result, thread_id=*((int*)args[0]), query_version;
	static int var=0;
	JobData *jobData=(JobData *)args[1];
	DynamicData *dData=(DynamicData *)args[2];
	uint32_t from, to, turn;
	
	from=jobData->from;		// Get the job
	to=jobData->to;
	turn=jobData->turn;
	query_version=jobData->version;
	var++;		// We use var for the visited array in bbfs

	// If there is a path between nodes components
	if (bbfs(dData->compGraph->indexOut, dData->compGraph->indexIn, dData->compGraph->bufOut, dData->compGraph->bufIn, var, dData->ccindex[from], dData->ccindex[to],thread_id, query_version)!=-1) {
		var++;
		// Check if there is a path 
		result=bbfs(dData->mainGraph->indexOut, dData->mainGraph->indexIn, dData->mainGraph->bufOut, dData->mainGraph->bufIn,var,from,to,thread_id,query_version);
	}
	else{	// Else -1
		result=-1;
	}

	js->result[turn]=result;		// Save result in position:turn to print results in correct order
	
	if (pthread_mutex_lock(&count_mtx)) printf("pthread_mutex_lock");		// Lock mutex for jobs_count
	jobs_count--;
	if(jobs_count==0)														// Awake other thread
		pthread_cond_signal(&empty_queue);
	if (pthread_mutex_unlock(&count_mtx)) printf("pthread_mutex_unlock");	// Unlock mutex for jobs_count
}

int static_query(void **args) {				// This function is executed by a thread in case of static graph 
	int need_bbfs=MAYBE, answer, result, thread_id=*((int*)args[0]);
	static int var=0;
	JobData *jobData=(JobData *)args[1];
	StaticData *sData=(StaticData *)args[2];
	uint32_t source_comp, target_comp, i, from, to, turn;
	
	from=jobData->from;		// Get the job
	to=jobData->to;
	turn=jobData->turn;
	var++;
	source_comp=findNodeStronglyConnectedComponentID(sData->scc, from);	// Find component of source node
	target_comp=findNodeStronglyConnectedComponentID(sData->scc, to);		// Find component of destination node
	need_bbfs=MAYBE;
	for(i=0; i<NUM_OF_GRAILS; i++) {		// Check grails
		answer=isReachableGrailIndex(sData->grailIndex[i], source_comp, target_comp);
		if (answer==YES) {		// If there is a yes find the path
			result=estimateShortestPathStronglyConnectedComponents(sData->scc,sData->mainGraph->indexOut,sData->mainGraph->indexIn,sData->mainGraph->bufOut,sData->mainGraph->bufIn,var,from,to,sData->grailIndex[i],thread_id);
			need_bbfs=YES;
			break;
		}
		else if(answer==NO) {	// If there is a no result=-1
			result=-1;
			need_bbfs=NO;
			break;
		}
	}
	if(need_bbfs==MAYBE) {		// If all the answers are MAYBE check if there is a path
		result=estimateShortestPathStronglyConnectedComponents(sData->scc,sData->mainGraph->indexOut,sData->mainGraph->indexIn,sData->mainGraph->bufOut,sData->mainGraph->bufIn, var,from,to,sData->grailIndex[0],thread_id);
	}
	js->result[turn]=result;
	if (pthread_mutex_lock(&count_mtx)) printf("pthread_mutex_lock");		// Lock mutex for jobs_count
	jobs_count--;
	if(jobs_count==0)														// Awake other thread
		pthread_cond_signal(&empty_queue);
	if (pthread_mutex_unlock(&count_mtx)) printf("pthread_mutex_unlock");	// Unlock mutex for jobd_count
}

void print_results() {
	uint32_t i;
	for(i=0; i<js->count; i++)
		printf("%d\n", js->result[i]);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include "graph.h"
#include "SCC.h"
#include "jsch.h"
#include "bbfs.h"
#include "WCC.h"
#include "grail.h"

JobScheduler* js;
uint32_t  max=0, q=0, jobs_count=0, exit_thread=0, sizeOfIndexIn=100, sizeOfIndexOut=100, sizeOfHash=100;
NodeIndex *sccIndex;
BufferInfo *sccInfo;
ListNode *sccBuf;
JobScheduler *js;
pthread_mutex_t thr_mtx, count_mtx, job_mtx;
pthread_cond_t empty_queue, full_queue;

int main(int argc, char **argv){
	NodeIndex *indexIn, *indexOut, *index1, *index2;
	ListNode *bufIn, *bufOut, *buf1, *buf2;
	BufferInfo *bufInInfo, *bufOutInfo, *info1, *info2;
	StaticData* sData;
	DynamicData *dData, *compGraph=malloc(sizeof(DynamicData));
	SCC* scc;
	Job *t=malloc(sizeof(Job));
	GrailIndex *grailIndex[NUM_OF_GRAILS];
	FILE *graph_file, *workload_file;
	char *graph_line=NULL, *graph_token, *workload_line=NULL, *workload_token, prev_cmd='A';
	size_t graph_len=0, workload_len=0;
	uint32_t id1, id2, k=1, j, turn=0, current_version=0;
	int i, result, flag=0, startNode[NUM_OF_GRAILS], startExist;
	int (*function)(void**);
	void** args;
	JobData *jobData;
	
	pthread_mutexattr_t Attr;
	pthread_mutexattr_init(&Attr);
	pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
	
	pthread_mutex_init(&thr_mtx,&Attr);
	pthread_mutex_init(&count_mtx,&Attr);
	pthread_mutex_init(&job_mtx,&Attr);
	pthread_cond_init(&empty_queue, NULL);
	pthread_cond_init(&full_queue, NULL);

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Check arguments ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if (argc!=5) {
		printf("Wrong number of arguments. Please, try again.\n");
		return -1;
	}
	for(i=1; i<argc; i+=2) {
		if (!strcmp("-g",argv[i]))
			graph_file=fopen(argv[i+1], "rb");
		if (!strcmp("-w",argv[i]))
			workload_file=fopen(argv[i+1], "rb");
	}

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Dynamic or Static graph? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if(getline(&workload_line, &workload_len, workload_file)!=-1) {
		workload_token=strtok(workload_line, " 	\n");   
		if(strcmp(workload_token, "DYNAMIC")==0) flag=1;
	}

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Create graph ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if (flag) {
		dData=malloc(sizeof(DynamicData));
		dData->mainGraph=createGraph(dData->mainGraph, sizeOfIndexOut, sizeOfIndexIn);
	}
	else {
		sData=malloc(sizeof(StaticData));
		sData->mainGraph=createGraph(sData->mainGraph, sizeOfIndexOut, sizeOfIndexIn);
	}

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~ Read commands from graph file ~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	while(getline(&graph_line, &graph_len, graph_file)!=-1) {   // take each line of file
		graph_token=strtok(graph_line, " 	\n");   // take the token until the space
		if(strcmp(graph_token, "S")==0)
			break;
		id1=atoi(graph_token);
		if(max<id1)	max=id1;
		graph_token=strtok (NULL, " 	\n");
		id2=atoi(graph_token);
		if(max<id2)	max=id2;
		if (flag)
			dData->mainGraph=insertGraph(dData->mainGraph, id1, id2, current_version, &sizeOfIndexOut, &sizeOfIndexIn);
		else
			sData->mainGraph=insertGraph(sData->mainGraph, id1, id2, current_version, &sizeOfIndexOut, &sizeOfIndexIn);
	}

	for(i=0; i<NUM_OF_THREADS; i++) {
		visitedOut[i]=Id_Create(visitedOut[i],&outSize[i]);
		visitedIn[i]=Id_Create(visitedIn[i],&inSize[i]);
		id_nodesOut[i]=Id_nodesCreate(id_nodesOut[i]);
		id_nodesIn[i]=Id_nodesCreate(id_nodesIn[i]);
	}

	if(flag==1) {
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~ Init & Create for Dynamic Graph ~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		stackWCC=initStack(stackWCC,max+1);
		visited=malloc((max+1)*sizeof(uint32_t));
		prevSize=max+1;
		memset(visited, 0, (max+1)*sizeof(uint32_t));
		ccindex=malloc((max+1)*sizeof(uint32_t));
		compNum=DFS(dData->mainGraph, 0, max+1);
		dData->compGraph=createGraph(dData->compGraph, compNum, compNum);
		dData->ccindex=ccindex;
		initialize_scheduler();
		if(pthread_mutex_lock(&job_mtx)) printf("pthread_mutex_lock");		// Lock mutex

		/*~~~~~~~~~~~~~~~~~~~~~~~~~~ Read commands from workload file ~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		while(getline(&workload_line, &workload_len, workload_file) != -1) {   // take each line of file
			workload_token=strtok(workload_line, " 	\n");   // take the token until the space
			if(!strcmp(workload_token, "A")) {
				if(prev_cmd=='Q') {
					prev_cmd='A';
					current_version++;
				}
				workload_token=strtok (NULL, " 	\n");
				id1=atoi(workload_token);
				workload_token=strtok (NULL, " 	\n");
				id2 = atoi(workload_token);
				if ((max<id1) || (max<id2)) {
					if (max<id1) max=id1;
					else max=id2;
					ccindex=(uint32_t*) realloc(ccindex, (max+1)*sizeof(uint32_t));
					memset(ccindex, 0, (max+1)*sizeof(uint32_t));
					compNum=DFS(dData->mainGraph, 0, max+1);
					destroyGraph(dData->compGraph);
					dData->compGraph=createGraph(dData->compGraph, compNum, compNum);
					dData->ccindex=ccindex;
				}
				if (!neighbors(dData->mainGraph->bufOut,dData->mainGraph->indexOut[id1].pos, dData->mainGraph->indexOut[id1].head, dData->mainGraph->indexOut[id1].tail, id1, id2)){
						dData->mainGraph=insertGraph(dData->mainGraph, id1, id2, current_version, &sizeOfIndexOut, &sizeOfIndexIn);
						dData->compGraph=insertGraph(dData->compGraph, dData->ccindex[id1], dData->ccindex[id2], current_version, &compNum, &compNum);
				}
				dData->ccindex=ccindex;
			}
			else if (!strcmp(workload_token, "Q")) {
				prev_cmd='Q';
				workload_token = strtok (NULL, " 	\n");
				id1=atoi(workload_token);
				workload_token = strtok (NULL, " 	\n");
				id2=atoi(workload_token);
		
				function=malloc(sizeof(int*));
				function=&dynamic_query;

				jobData=malloc(sizeof(JobData));
				jobData->from=id1;
				jobData->to=id2;
				jobData->turn=turn;
				jobData->version=current_version;

				args=malloc(3*sizeof(void*));
				args[1]=(void *)jobData;
				args[2]=(void *)dData;
				submit_job(function, args);
				turn++;
			}
			else if (!strcmp(workload_token, "F")) {
				execute_all_jobs();
				wait_all_tasks_finish();

				print_results();
				turn=0;
				js->q->count=0;
				js->q->first=0;
				for (i=js->q->first; i<js->q->count; i++){
					free(js->q->job[i].args);
					free(js->q->job[i].function);
				}
			}
		}
		if (pthread_mutex_unlock(&job_mtx)) printf("pthread_mutex_unlock");	// Unlock mutex
		destroyGraph(dData->compGraph);
		free(ccindex);
		freeStack(stackWCC);
		destroyGraph(dData->mainGraph);
	}
	else {
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~ Init & Create for Static Graph ~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		
		srand(time(NULL));
		scc=estimateStronglyConnectedComponents(sData, 0, max+1);	// tarjan's algorithm to find scc
		sData->scc=scc;
		
		sccIndex=createNodeIndex(sccIndex,sccComponents);
		sccInfo=createBufferInfo(sccInfo, 5);
		sccBuf=createBuffer(sccBuf, 5);

		findStronglyConnectedComponentsNeighbors(sData->scc, sData->mainGraph->indexOut, sData->mainGraph->bufOut, sData->mainGraph->bufOutInfo);	// create hypergraph
		
		createGrailThings();

		for(i=0; i<NUM_OF_GRAILS; i++) {
			do {
				startNode[i]=rand() % sccComponents;
				startExist=0;
				for(j=0; j<i; j++) {
					if(startNode[j]==startNode[i]) {
						startExist=1;
						break;
					}
				}
			} while(startExist==1);
			sData->grailIndex[i]=buildGrailIndex(startNode[i], i+1);
		}
		initialize_scheduler();
		if(pthread_mutex_lock(&job_mtx)) printf("pthread_mutex_lock");		// Lock mutex
		
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~ Read commands from workload file ~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		while(getline(&workload_line, &workload_len, workload_file) != -1) {	// take each line of file
			workload_token=strtok(workload_line, " 	\n");   						// take the token until the space
			if (!strcmp(workload_token, "Q")) {	// if the command is a query
				q++;
				workload_token=strtok (NULL, " 	\n");
				id1=atoi(workload_token);
				workload_token=strtok (NULL, " 	\n");
				id2=atoi(workload_token);
				//	give the scheduler the data it needs to get the new job
				function=malloc(sizeof(int*));
				args=malloc(3*sizeof(void*));		//these are the arguements the threads need to execute the current job
				
				function=&static_query;				//this is the function that the threads will execute

				jobData=malloc(sizeof(JobData));
				jobData->from=id1;
				jobData->to=id2;
				jobData->turn=turn;

				args[1]=(void *)jobData;
				args[2]=(void *)sData;
				submit_job(function, args);
				
				turn++;									// turn is used so the results are printed in the correct order
			}
			else if(!strcmp(workload_token, "F")) {
				execute_all_jobs();					// execute all jobs
				wait_all_tasks_finish();			// wait the jobs to finish before reading new commands
				print_results();						// print results
				turn=0;
				js->q->count=0;
				js->q->first=0;
				for (i=js->q->first; i<js->q->count; i++){
					free(js->q->job[i].args);
					free(js->q->job[i].function);
				}
			}
		}
		if (pthread_mutex_unlock(&job_mtx)) printf("pthread_mutex_unlock");	// Unlock mutex

		destroyStronglyConnectedComponents(scc);
		destroyNodeIndex(sccIndex);
		destroyBuffer(sccBuf, sccInfo);
		freeGrailThings();
		for(i=0; i<NUM_OF_GRAILS; i++)
			destroyGrailIndex(sData->grailIndex[i]);
		destroyGraph(sData->mainGraph);
	}

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~ Free the allocated space ~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if(destroy_scheduler()==FAIL)
			printf("Error in destroy_scheduler\n");
	for(i=0; i<NUM_OF_THREADS; i++) {
		Id_Free(visitedOut[i]);
		Id_Free(visitedIn[i]);
		Id_nodesFree(id_nodesOut[i]);
		Id_nodesFree(id_nodesIn[i]);
	} 
	fclose(workload_file);
	fclose(graph_file);
	return 0;
}

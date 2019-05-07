#ifndef JSCH_H
#define JSCH_H

#include <stdint.h>
#include <pthread.h>

typedef struct JobData {
	uint32_t from;					// Source node
	uint32_t to;					// Destination node
	uint32_t turn;					// Defines next job
	uint32_t version;				// Static-dynamic
} JobData;

typedef struct Job {				// Struct for each job
	int (*function)(void **);	// Function for job execution
	void** args;					// Arguments of function
} Job;

typedef struct Queue {
	uint32_t size;					// Max size of queue
	int first;						// First element in queue
	uint32_t count;				// Current num of jobs in queue
	Job *job;						// Queue of jobw
} Queue;

typedef struct JobScheduler {
	int execution_threads; 		// Number of execution threads
	Queue* q; 						// A queue that holds submitted jobs/tasks
	pthread_t* tids;				// Execution threads
	uint32_t size;					// Max size of queue
	uint32_t count;				// Number of jobs waiting
	int *result;					// Array of ripis results
}JobScheduler;


void initialize_scheduler();
void submit_job(void* function, void** args);
void execute_all_jobs();
void wait_all_tasks_finish(); 
OK_SUCCESS destroy_scheduler();

void initQ(int qsize);
void* thread_function();
int dynamic_query(void **args);
int static_query(void **args);
void print_results();

#endif

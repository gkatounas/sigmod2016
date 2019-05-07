#ifndef DEFINE_H
#define DEFINE_H

#define NEIGHBORS 10				// Number of neighbors in every listnode
#define BUCKETSIZE 10			//	Size in hash table, number of buckets that we use and do mod with the id of node
#define BUCKET_NUM 4				// The initial size we have in every bucket
#define SIZE 100					// Size than we use in queue of job scheduler and in other structs
#define NUM_OF_THREADS 4		// Number of threads
#define NUM_OF_GRAILS 5			// Number of grail indexes

typedef enum OK_SUCCESS {		// Enumeration that shows the return of some functions
	SUCCESS=1,
	FAIL=0
} OK_SUCCESS;

#endif

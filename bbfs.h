#ifndef BBFS_H
#define BBFS_H

#include "index.h"

uint32_t *visitedOut[NUM_OF_THREADS];				// Array with nodes to know which we have visited
uint32_t *visitedIn[NUM_OF_THREADS];				// The same but for incoming nodes

uint32_t outSize[NUM_OF_THREADS];					// The initial size of this array for the outgoing nodes
uint32_t inSize[NUM_OF_THREADS];						// The same for incoming
	
uint32_t outChildren[NUM_OF_THREADS];				// Counter for the children that we will visit at the next level
uint32_t inChildren[NUM_OF_THREADS];				// The same for the other side

typedef struct Id_nodes {								// Struct in which we keep the nodes that we have visited 
	ptr *array;												// in this array
	ptr size;												// Initial size of this array
	ptr counter;											// Counter for the elements in this array
	ptr first;
	ptr last;
} Id_nodes;

Id_nodes *id_nodesOut[NUM_OF_THREADS];				// One array for the outgoing nodes
Id_nodes *id_nodesIn[NUM_OF_THREADS];				// and one for the incoming nodes

uint32_t* Id_Create(uint32_t *visited,uint32_t *visitedSize);	// Creates the visited arrays that be used in bbfs algorithm
Id_nodes* Id_nodesCreate(Id_nodes *id_nodes);
uint32_t* Id_Init(uint32_t *Id_level,NodeIndex *index,uint32_t *size,uint32_t *children,ptr id,uint32_t query);	// It's called every time that we have a Q and make the values for first nodes at level arrays
Id_nodes* Id_nodesInit(Id_nodes *id_nodes,ptr id);	// It's called every time that we have a Q and initializes the arrays inside the structs
uint32_t* Id_Increase(uint32_t id,uint32_t *arraySize, uint32_t *array);	// Increase the array
Id_nodes* Id_nodesIncrease();
void Id_Free(uint32_t *array);						// Free the visited arrays
void Id_nodesFree(Id_nodes *id_nodes);				// Free the Id_nodes structs
int bbfs(NodeIndex *index1,NodeIndex *index2,ListNode *buf1,ListNode *buf2,uint32_t query,uint32_t id1,uint32_t id2,int thread_id, uint32_t query_version); // Implementation of bbfs algorithm

#endif

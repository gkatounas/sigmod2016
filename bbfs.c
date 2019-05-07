#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bbfs.h"


uint32_t* Id_Create(uint32_t *visited,uint32_t *visitedSize) {	// Allocate memory, called only once from main
	visited=malloc(SIZE*sizeof(uint32_t));			// Allocation of memory for the array with the initial size SIZE which is defined
	memset(visited, 0, SIZE);							// Initialization of the array with initial value o
	(*visitedSize)=SIZE;									// Save the initial size of the arrays at a variable for future changes
	return visited;
}

Id_nodes* Id_nodesCreate(Id_nodes *id_nodes) {
	id_nodes=malloc(sizeof(Id_nodes));				// Allocate memory for the first struct that we need to keep the nodes we have visited
	id_nodes->array=malloc(SIZE*sizeof(ptr));		// Allocate memory for the array in which we save the nodes that we have visited
	id_nodes->size=SIZE;									// Equal the size of the array with SIZE, which we used to allocate the memory for the array
	return id_nodes;	
}

uint32_t* Id_Init(uint32_t *visited,NodeIndex *index, uint32_t *visitedSize, uint32_t *sizeChildren, uint32_t id, uint32_t query) {
	if(id>=(*visitedSize)) {							// Check if if number > size of visited array, then increase the array
		visited=Id_Increase(id, &(*visitedSize), visited);
	}
	visited[id]=query;									// The current number that be used in visited array
	if(index[id].head != -1)							// If the node has children
		(*sizeChildren)=index[id].tail*NEIGHBORS+index[id].pos;	// then we save the number of them
	else
		(*sizeChildren)=0;
	return visited;
}

Id_nodes* Id_nodesInit(Id_nodes *id_nodes,ptr id){		// It's called every that we call bbfs and initialize the struct Id_nodes
	id_nodes->array[0]=id;										// Put at the first position the value equal to the first node
	id_nodes->counter=1;											// Increase the counter of the arrays' elements
	id_nodes->first=0;											// First element is at 0 position
	id_nodes->last=1;												// last shows the next available position
	return id_nodes;
}

int bbfs(NodeIndex *index1,NodeIndex *index2,ListNode *buf1,ListNode *buf2,uint32_t query,uint32_t id1,uint32_t id2, int thread_id, uint32_t query_version) {	// Bbfs algorithm
	uint32_t node, position, i, j;
	int stop=0;

	ptr level1=0, level2=0;						// Initialize the first level with zero

	if (id1==id2) return 0;					// Check if the nodes are the same, returning 0 at this case
	visitedOut[thread_id]=Id_Init(visitedOut[thread_id],index1,&outSize[thread_id],&outChildren[thread_id],id1,query);// Put the first values at the proper fields of the arrays and inits the variables
	visitedIn[thread_id]=Id_Init(visitedIn[thread_id],index2,&inSize[thread_id],&inChildren[thread_id],id2,query);
	id_nodesOut[thread_id]=Id_nodesInit(id_nodesOut[thread_id],id1);	// Add the nodes at the array that shows which nodes we have visited
	id_nodesIn[thread_id]=Id_nodesInit(id_nodesIn[thread_id],id2);

	while(1) {
		if(outChildren[thread_id]<inChildren[thread_id]) {	// Start from the side with the less children
			outChildren[thread_id]=0;				// Initialize the counter of the children again
			while(id_nodesOut[thread_id]->first<id_nodesOut[thread_id]->last) {	// Add the children of all nodes at this level
				node=id_nodesOut[thread_id]->array[id_nodesOut[thread_id]->first++];// Take the first element from the queue
				position=index1[node].head;		// Start from the first listnode
				while(position != index1[node].tail && stop==0) {	// Search at all listnodes until the last or if found one than has been added later
					for(i=0; i<NEIGHBORS; i++) {	// Take all the neighbors
						if(buf1[position].edgeProperty[i]>query_version) {	// Check if the childnode has been added after the query
							stop=1;
							break;
						}
						if(buf1[position].neighbor[i]>=inSize[thread_id])	// Check if we need to increase the
							visitedIn[thread_id]=Id_Increase(buf1[position].neighbor[i], &inSize[thread_id], visitedIn[thread_id]);	
						if(visitedIn[thread_id][buf1[position].neighbor[i]]==query)	// If it's visited
							return level1+level2+1;									// Return the path if we found the node
						if(buf1[position].neighbor[i]>=outSize[thread_id])
							visitedOut[thread_id]=Id_Increase(buf2[position].neighbor[i], &outSize[thread_id], visitedOut[thread_id]);
						if(visitedOut[thread_id][buf1[position].neighbor[i]]!=query){	// Otherwise we continue the search to the listnode
							visitedOut[thread_id][buf1[position].neighbor[i]]=query;	// Giving the right level
							if(id_nodesOut[thread_id]->counter>=id_nodesOut[thread_id]->size){// Check if it's suits at the array with the visited nodes
								id_nodesOut[thread_id]=Id_nodesIncrease(id_nodesOut[thread_id]);		// renewing the size
							}
							id_nodesOut[thread_id]->array[id_nodesOut[thread_id]->counter++]=buf1[position].neighbor[i];	// Add it there
							if(index1[buf1[position].neighbor[i]].head != -1)// Add the number of it's children if it has
								outChildren[thread_id]+=index1[buf1[position].neighbor[i]].tail*NEIGHBORS+index1[buf1[position].neighbor[i]].pos;
						}
					}
					position=buf1[position].nextListNode;	// Continue the search with the next listnode
				}
				if(stop==0) {		// If we haven't found any node that has been added after query
					for(i=0; i<index1[node].pos; i++) {	// Search at the last listnode until the position that is not empty
						if(buf1[position].edgeProperty[i]>query_version) {
							break;
						}
						if(buf1[position].neighbor[i]>=inSize[thread_id])
							visitedIn[thread_id]=Id_Increase(buf1[position].neighbor[i], &inSize[thread_id], visitedIn[thread_id]);	// doing exactly the same process
						if(visitedIn[thread_id][buf1[position].neighbor[i]]==query)
							return level1+level2+1;
						if(buf1[position].neighbor[i]>=outSize[thread_id])
							visitedOut[thread_id]=Id_Increase(buf2[position].neighbor[i], &outSize[thread_id], visitedOut[thread_id]);
						if(visitedOut[thread_id][buf1[position].neighbor[i]]!=query) {
							visitedOut[thread_id][buf1[position].neighbor[i]]=query;
							if(id_nodesOut[thread_id]->counter>=id_nodesOut[thread_id]->size){
								id_nodesOut[thread_id]=Id_nodesIncrease(id_nodesOut[thread_id]);
							}
							id_nodesOut[thread_id]->array[id_nodesOut[thread_id]->counter++]=buf1[position].neighbor[i];
							if(index1[buf1[position].neighbor[i]].head != -1)
								outChildren[thread_id]+=index1[buf1[position].neighbor[i]].tail*NEIGHBORS+index1[buf1[position].neighbor[i]].pos;
						}
					}
				}
				else	stop=0;
			}
			if(id_nodesOut[thread_id]->counter==id_nodesOut[thread_id]->first) return -1;
			id_nodesOut[thread_id]->last=id_nodesOut[thread_id]->counter;
			level1++;							// Increase the level of graph
		}
		else{									// Exactly the same with above for the other side of the graph
			inChildren[thread_id]=0;
			while(id_nodesIn[thread_id]->first<id_nodesIn[thread_id]->last) {	// pros8etoume ta paidia olwn twn komvwn tou epipedou
				node=id_nodesIn[thread_id]->array[id_nodesIn[thread_id]->first++];
				position=index2[node].head;
				while(position != index2[node].tail && stop==0) {
					for(i=0; i<NEIGHBORS; i++) {
						if(buf2[position].edgeProperty[i]>query_version) {
							stop=1;
							break;
						}
						if(buf2[position].neighbor[i]>=outSize[thread_id])
							visitedOut[thread_id]=Id_Increase(buf2[position].neighbor[i], &outSize[thread_id], visitedOut[thread_id]);
						if(visitedOut[thread_id][buf2[position].neighbor[i]]==query)
							return level1+level2+1;
						if(buf2[position].neighbor[i]>=inSize[thread_id])
							visitedIn[thread_id]=Id_Increase(buf2[position].neighbor[i], &inSize[thread_id], visitedIn[thread_id]);
						if(visitedIn[thread_id][buf2[position].neighbor[i]]!=query) {
							visitedIn[thread_id][buf2[position].neighbor[i]]=query;
							if(id_nodesIn[thread_id]->counter>=id_nodesIn[thread_id]->size){
								id_nodesIn[thread_id]=Id_nodesIncrease(id_nodesIn[thread_id]);
							}
							id_nodesIn[thread_id]->array[id_nodesIn[thread_id]->counter++]=buf2[position].neighbor[i];
							if(index2[buf2[position].neighbor[i]].head != -1)
								inChildren[thread_id]+=index2[buf2[position].neighbor[i]].tail*NEIGHBORS+index2[buf2[position].neighbor[i]].pos;
						}
					}
					position=buf2[position].nextListNode;
				}
				if(stop==0) {
					for(i=0; i<index2[node].pos; i++) {
						if(buf2[position].edgeProperty[i]>query_version) {
							
							break;
						}
						if(buf2[position].neighbor[i]>=outSize[thread_id])
							visitedOut[thread_id]=Id_Increase(buf2[position].neighbor[i], &outSize[thread_id], visitedOut[thread_id]);
						if(visitedOut[thread_id][buf2[position].neighbor[i]]==query)
							return level1+level2+1;
						if(buf2[position].neighbor[i]>=inSize[thread_id])
							visitedIn[thread_id]=Id_Increase(buf2[position].neighbor[i], &inSize[thread_id], visitedIn[thread_id]);
						if(visitedIn[thread_id][buf2[position].neighbor[i]]!=query) {
							visitedIn[thread_id][buf2[position].neighbor[i]]=query;
							if(id_nodesIn[thread_id]->counter>=id_nodesIn[thread_id]->size){
								id_nodesIn[thread_id]=Id_nodesIncrease(id_nodesIn[thread_id]);
							}
							id_nodesIn[thread_id]->array[id_nodesIn[thread_id]->counter++]=buf2[position].neighbor[i];
							if(index2[buf2[position].neighbor[i]].head != -1)
								inChildren[thread_id]+=index2[buf2[position].neighbor[i]].tail*NEIGHBORS+index2[buf2[position].neighbor[i]].pos;
						}
					}
				}
				else	stop=0;
			}
			if(id_nodesIn[thread_id]->counter==id_nodesIn[thread_id]->first) return -1;
			id_nodesIn[thread_id]->last=id_nodesIn[thread_id]->counter;
			level2++;
		}
	}
}

uint32_t* Id_Increase(ptr id, ptr *arraySize, uint32_t *array) {	// Check & increase the size of the visited array
	ptr size, number;							// and in which level
	if(id>2*(*arraySize))				// If true, then we compare the id with the double of the size
		size=2*id;							// If id is bigger, we make the new size equal to the double value of id
	else  
		size=2*(*arraySize);				// otherwise equal with the double value of the existed size
	number=size-(*arraySize);			// Find the number of the new array's cells
	array=(uint32_t*) realloc(array, size*sizeof(uint32_t));	// Realloc the array with the new size
	memset(array+(*arraySize), 0, number*sizeof(uint32_t));			// Use memset to initialize the values at the new cells
	(*arraySize)=size;					// Renew the size of the array
	return array;							// Return the new array
}

Id_nodes* Id_nodesIncrease(Id_nodes *id_nodes) {						// Increase the size of array inside the Id_nodes struct
	id_nodes->array=(ptr *) realloc(id_nodes->array, (2*id_nodes->size)*(sizeof(ptr)));
	id_nodes->size=2*id_nodes->size;
	return id_nodes;
}

void Id_Free(uint32_t *array) {								// Free the allocated memory, is called only once from main at the end
	free(array);
}

void Id_nodesFree(Id_nodes *id_nodes) {					// Free the allocated memory of Id_nodes structs 
	free(id_nodes->array);
	free(id_nodes);
}

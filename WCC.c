#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "stack.h"
#include "WCC.h"
#include "bbfs.h"

extern uint32_t *compCounter, compNum, *visited, prevSize, *ccindex;
int k=1, d=1;

uint32_t DFS(Graph* dgraph, uint32_t start, uint32_t nodes) {
	uint32_t temp,temp1,tail,pos,component=-1,counter=0,i,undefined=0;

	if (nodes>prevSize) {
		visited=(uint32_t*) realloc(visited, nodes*sizeof(uint32_t));
		prevSize=nodes;
	}
	stackWCC->counter=0;
	stackWCC=pushStack(stackWCC,start);				// Push start node in stack
	visited[start]=k;										// Mark start node as visited
	while (counter!=nodes) {                		// While we haven't visited all nodes
		component++;										// Every time stack gets empty we have one wcc 
		while (!isEmpty(stackWCC)) {             	// While we are in one component
			temp = topStack(stackWCC);					// Get the top element from stack
			stackWCC=popStack(stackWCC);				// Decrease stack size
			counter++;										// Increase counter of nodes
			ccindex[temp]=component;					// Save component of current node in struct ccindex 
			temp1 = dgraph->indexOut[temp].head;	
			tail=dgraph->indexOut[temp].tail;		
			pos=dgraph->indexOut[temp].pos;			
			pushNeighbors(temp1, tail, pos, dgraph->bufOut);		// Push outcomimg neighbours of node in stack
			
			temp1 = dgraph->indexIn[temp].head;
			tail=dgraph->indexIn[temp].tail;
			pos=dgraph->indexIn[temp].pos;
			pushNeighbors(temp1, tail, pos, dgraph->bufIn);			// Push incomimg neighbours of node in stack
		}
		for (i=undefined; i<nodes; i++) {         // Push the first unvisited node in stack to start a new component
			if (visited[i]!=k) {							// We use k to avoid initialization of visited array
				stackWCC=pushStack(stackWCC,i);
				visited[i]=k;
				undefined=i;
				break;
			}
    	}
	}
	k++;
	return component+1;
}

void pushNeighbors(uint32_t temp, uint32_t tail, uint32_t pos, ListNode *buf) {
	uint32_t i;
	
	while (temp<tail) {									// For each listnode
		for (i=0; i<NEIGHBORS; i++) {  				// For each neighbour
			if (visited[buf[temp].neighbor[i]]!=k) {	// If we haven't visited him
				visited[buf[temp].neighbor[i]]=k;		// Mark him as visited
				stackWCC=pushStack(stackWCC,buf[temp].neighbor[i]);    // and push him in stack
			}
		}
		temp = buf[temp].nextListNode;
	}
	for (i=0; i<pos; i++){								// Do the same for last listnode
		if (visited[buf[temp].neighbor[i]]!=k) {
			visited[buf[temp].neighbor[i]]=k;
			stackWCC=pushStack(stackWCC,buf[temp].neighbor[i]);    
		}
	}

}

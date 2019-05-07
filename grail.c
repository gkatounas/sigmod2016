#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grail.h"

extern NodeIndex *sccIndex;
extern BufferInfo *sccInfo;
extern ListNode *sccBuf;
extern uint32_t sccComponents;

void createGrailThings() {							// Create and init the stack and visited array that use to create the grailIndex
	grailVisited=malloc(sccComponents*sizeof(uint32_t));
	memset(grailVisited,0,sccComponents*sizeof(uint32_t));
	grailStack=initStack(grailStack,20);		// Call the function of stack tou init the grailStack
}

GrailIndex* buildGrailIndex(uint32_t start, int n) {
	GrailIndex *index=malloc(sccComponents*sizeof(GrailIndex));
	uint32_t id, rank=1, i, j=start, s=start, temp, limit=sccComponents;
	
	for(i=0; i<sccComponents; i++)			// Init the field min_rank of each component with a very big value
		index[i].min_rank=sccComponents;
	grailStack->counter=0;							// Every time we init the size of stack equal to 0, to use the same struct
	grailStack=pushStack(grailStack,start);// Push in stack the first node, by random
	while(j<limit) {
		while(!isEmpty(grailStack)) {
			id=topStack(grailStack);
			if(grailVisited[id]!=n) {					// If we haven't visited the node id yet
				grailVisited[id]=n;
				temp=sccIndex[id].head;
				if(temp!=-1) {								// If the node has children
					while(temp!=sccIndex[id].tail) {
						for(i=0; i<NEIGHBORS; i++) {	// Take each child
							if(grailVisited[sccBuf[temp].neighbor[i]]!=n)	// Check if it's visited
								grailStack=pushStack(grailStack, sccBuf[temp].neighbor[i]);	// if not then put it in stack
						}
						temp=sccBuf[temp].nextListNode;
					}
					for(i=0; i<sccIndex[id].pos; i++) {
						if(grailVisited[sccBuf[temp].neighbor[i]]!=n)
							grailStack=pushStack(grailStack, sccBuf[temp].neighbor[i]);
					}
				}
				else {										// If the node doesn't have any child
					index[id].min_rank=rank;			// Do its min_rank equal to the current rank
					index[id].rank=rank;					// The same for its rank
					grailStack=popStack(grailStack);	// Pop it from the stack
					rank++;									// and increase the rank
				}
			}
			else {											// If the node is visited 
				temp=sccIndex[id].head;
				while(temp!=sccIndex[id].tail) {		// Take its child 
					for(i=0; i<NEIGHBORS; i++) {
						if(index[sccBuf[temp].neighbor[i]].min_rank<index[id].min_rank)	// and compare its min_rank
							index[id].min_rank=index[sccBuf[temp].neighbor[i]].min_rank;	// if its less than node's min_rank, save it
					}
					temp=sccBuf[temp].nextListNode;
				}
				for(i=0; i<sccIndex[id].pos; i++) {
					if(index[sccBuf[temp].neighbor[i]].min_rank<index[id].min_rank)
						index[id].min_rank=index[sccBuf[temp].neighbor[i]].min_rank;
				}
				index[id].rank=rank;						// Do its min_rank equal to the current rank 
				rank++;										// and increase the rank
				grailStack=popStack(grailStack);		// Pop the node from the stack
			}
		}
		for(j=s+1; j<limit; j++) {
			if(grailVisited[j]!=n) {		// Take the next node that we haven't visited yet
				s=j;								// Change it for the next time, not to do more iterations
				grailStack=pushStack(grailStack,j);
				break;
			}		
		}
		if(j==sccComponents) {		// Doesn't mean than we have visited all the components
			limit=start;				// because we maybe start from another node different than 0
			s=0;							// so we start again from zero to the previous first node
			j=0;
		}
	}	
	return index;
}

GRAIL_ANSWER isReachableGrailIndex(GrailIndex* index, uint32_t source_comp,uint32_t target_comp) {	// Takes the components of nodes
	if(source_comp==target_comp)	return YES;	// If nodes are in the same component then there is a path between them
	if((index[target_comp].min_rank>index[source_comp].rank) || (index[target_comp].rank<index[source_comp].min_rank)) {
		return NO;						// If this condition is not satisfying then there isn't a path between the nodes
	}
	else	return MAYBE;				// At any other case
}

void printGrailIndex(GrailIndex *index) {		// Print the grail index, each component with its min_rank and rank
	uint32_t i;
	for(i=0; i<sccComponents; i++)
		printf("Comp %d, min_rank %d, rank %d\n", i, index[i].min_rank, index[i].rank);
}

void freeGrailThings() {			// Free the memory that use the stack and the visited array of algorithm that make the grail index
	freeStack(grailStack);
	free(grailVisited);
}

int destroyGrailIndex(GrailIndex* index) {		// Free the allocated memory of grailIndex struct
	free(index);
}

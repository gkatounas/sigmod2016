#ifndef GRAIL_H
#define GRAIL_H

#include "index.h"
#include "stack.h"

typedef enum GRAIL_ANSWER {			// Enumeration for the answer that returns the grail
	NO=0,
	MAYBE=1,
	YES=2
} GRAIL_ANSWER;

typedef struct GrailIndex {			// Grail struct
	uint32_t min_rank;					// Label min_rank: the minimum value of children rank
	uint32_t rank;							// Label rank: the line of visiting this node
} GrailIndex;

Stack *grailStack;						//
uint32_t *grailVisited;					// Array for visited nodes

void createGrailThings();		// Function that creates and inits the stack and array that we use for grail index creation
GrailIndex* buildGrailIndex(uint32_t start, int n);	// Create a grailIndex
GRAIL_ANSWER isReachableGrailIndex(GrailIndex* index, uint32_t source_node, uint32_t target_node);	// Return the answer of grailIndex
void printGrailIndex(GrailIndex *index);	// Print the grail index, component id and min_rank and rank of each component
void freeGrailThings();	// Free the structs that uses the algorithm which creates the grail index
int destroyGrailIndex(GrailIndex* index);	// Free the allocated memory of grailIndex struct

#endif

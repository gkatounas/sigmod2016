#ifndef DFS_H
#define DFS_H

#include <stdint.h>
#include "index.h"
#include "stack.h"
#include "graph.h"

uint32_t *compCounter, compNum, *visited, prevSize, *ccindex;

typedef struct DynamicData {						// Struct for dynamic graphs 
	Graph *mainGraph, *compGraph;					// Graph of nodes and graph of components
	uint32_t *ccindex;								// Array that tells in which component belongs each node
} DynamicData;

ptr DFS(Graph* dgraph, ptr startNode, ptr nodes);
void pushNeighbors(uint32_t temp, uint32_t tail, uint32_t pos, ListNode *buf);

#endif

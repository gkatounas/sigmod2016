#ifndef GRAPH_H
#define GRAPH_H

#include "grail.h"
#include "index.h"
#include "buffer.h"

extern uint32_t sizeOfIndexIn, sizeOfIndexOut;

typedef struct Graph{									// Struct of the whole graph
	NodeIndex *indexIn, *indexOut;					
	ListNode *bufOut, *bufIn;							
	BufferInfo *bufInInfo, *bufOutInfo;
} Graph;

Graph* createGraph(Graph* d, uint32_t sizeOut, uint32_t sizeIn);
Graph* insertGraph(Graph* dgraph, uint32_t id1, uint32_t id2, uint32_t current_version, uint32_t* sizeOut, uint32_t* sizeIn);
void destroyGraph(Graph* dgraph);

#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "graph.h"

Graph* createGraph(Graph* d, uint32_t sizeOut, uint32_t sizeIn){	// Creation and initialization of graph
	d=malloc(sizeof(Graph));
	d->indexIn=createNodeIndex(d->indexIn,sizeIn);
	d->indexOut=createNodeIndex(d->indexOut,sizeOut);
	d->bufInInfo=createBufferInfo(d->bufInInfo, 5);
	d->bufOutInfo=createBufferInfo(d->bufOutInfo, 5);
	d->bufIn=createBuffer(d->bufIn, 5);
	d->bufOut=createBuffer(d->bufOut, 5);
	return d;
}

// Insert in indexes and buffers 
Graph* insertGraph(Graph* dgraph, uint32_t id1, uint32_t id2, uint32_t current_version, uint32_t* sizeOut, uint32_t* sizeIn){
	dgraph->indexOut=insertNodeIndex(dgraph->indexOut, id1, sizeOut, &dgraph->bufOutInfo->available);
	dgraph->indexIn=insertNodeIndex(dgraph->indexIn, id2, sizeIn, &dgraph->bufInInfo->available);
	dgraph->bufOut=allocNewNode(dgraph->bufOut, dgraph->bufOutInfo, &dgraph->indexOut[id1].pos, &dgraph->indexOut[id1].tail, id1, id2, current_version);
	dgraph->bufIn=allocNewNode(dgraph->bufIn, dgraph->bufInInfo, &dgraph->indexIn[id2].pos, &dgraph->indexIn[id2].tail, id2, id1, current_version);
		return dgraph;
}

void destroyGraph(Graph* dgraph){		// Destroy graph
	destroyNodeIndex(dgraph->indexIn);
	destroyNodeIndex(dgraph->indexOut);
	destroyBuffer(dgraph->bufIn, dgraph->bufInInfo);
	destroyBuffer(dgraph->bufOut, dgraph->bufOutInfo);
}

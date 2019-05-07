#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "graph.h"
#include "SCC.h"
#include "jsch.h"
#include "bbfs.h"
#include "WCC.h"
#include "grail.h"
//gcc -o utest utest.c index.c graph.c buffer.c bbfs.c to compile

int main(){
	//create a graph for testing
	DynamicData *dData;
	int i=0;
	char *graph_line=NULL, *graph_token;
	size_t graph_len=0;
	uint32_t indexOutSize=5,indexInSize=5,bufInSize=5,bufOutSize=5,current_version=0,id1,id2,max=0,query=1;
	FILE *graph_file=fopen("testGraph.txt","rb");
	NodeIndex *indexIn, *indexOut;
	ListNode *bufIn, *bufOut;
	BufferInfo *bufInInfo, *bufOutInfo;
	dData=malloc(sizeof(DynamicData));
	dData->mainGraph=createGraph(dData->mainGraph, indexOutSize, indexInSize);
	while(getline(&graph_line, &graph_len, graph_file)!=-1) {   // take each line of file
		graph_token=strtok(graph_line, " 	\n");   // take the token until the space
		if(strcmp(graph_token, "S")==0)
			break;
		id1=atoi(graph_token);
		if(max<id1)	max=id1;
		graph_token=strtok (NULL, " 	\n");
		id2=atoi(graph_token);
		if(max<id2)	max=id2;
		dData->mainGraph=insertGraph(dData->mainGraph, id1, id2, current_version, &indexOutSize, &indexInSize);
	}
	//print both incoming and outgoing edges
	printNodeIndex(dData->mainGraph->indexOut, indexOutSize, dData->mainGraph->bufOut,dData->mainGraph-> bufOutInfo);
	printf("-------------------------------------------------\n");
	printNodeIndex(dData->mainGraph->indexIn, indexInSize, dData->mainGraph->bufIn, dData->mainGraph->bufInInfo);
	for(i=0; i<1; i++) {
		visitedOut[i]=Id_Create(visitedOut[i],&outSize[i]);
		visitedIn[i]=Id_Create(visitedIn[i],&inSize[i]);
		id_nodesOut[i]=Id_nodesCreate(id_nodesOut[i]);
		id_nodesIn[i]=Id_nodesCreate(id_nodesIn[i]);
	}
	printf("bbfs:\n");
	id1=0;
	id2=6;
	int result=bbfs(dData->mainGraph->indexOut,dData->mainGraph->indexIn,dData->mainGraph->bufOut,dData->mainGraph->bufIn,query,id1,id2,0,current_version);//shortest path: 0->1->5->6
	query++;
	printf("result=%d\n",result);
	insertGraph(dData->mainGraph, 3, 6, 0, &indexOutSize, &indexInSize);//edge 3->6 was added so the new shortest path is 0->3->6 and we expect bbfs to return 3 instead of 4
	current_version++;
	result=bbfs(dData->mainGraph->indexOut,dData->mainGraph->indexIn,dData->mainGraph->bufOut,dData->mainGraph->bufIn,query,id1,id2,0,current_version);
	query++;
	printf("------------------\nresult=%d\n",result);
	destroyGraph(dData->mainGraph);
	for(i=0; i<1; i++) {
		Id_Free(visitedOut[i]);
		Id_Free(visitedIn[i]);
		Id_nodesFree(id_nodesOut[i]);
		Id_nodesFree(id_nodesIn[i]);
	} 
	free(dData);
}


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

NodeIndex *sccIndex;
BufferInfo *sccInfo;
ListNode *sccBuf;

int main(){
	StaticData *sData;
	int i=0;
	char *graph_line=NULL, *graph_token;
	size_t graph_len=0;
	uint32_t indexOutSize=5,indexInSize=5,bufInSize=5,bufOutSize=5,current_version=0,id1,id2,max=0,query=1;
	SCC* scc;
	FILE *graph_file=fopen("testGraph.txt","rb");
	NodeIndex *indexIn, *indexOut;
	ListNode *bufIn, *bufOut;
	BufferInfo *bufInInfo, *bufOutInfo;
	sData=malloc(sizeof(StaticData));
	sData->mainGraph=createGraph(sData->mainGraph, indexOutSize, indexInSize);


	sData=malloc(sizeof(StaticData));
	sData->mainGraph=createGraph(sData->mainGraph, indexOutSize, indexInSize);
	while(getline(&graph_line, &graph_len, graph_file)!=-1) {   // take each line of file
		graph_token=strtok(graph_line, " 	\n");   // take the token until the space
		if(strcmp(graph_token, "S")==0)
			break;
		id1=atoi(graph_token);
		if(max<id1)	max=id1;
		graph_token=strtok (NULL, " 	\n");
		id2=atoi(graph_token);
		if(max<id2)	max=id2;
		sData->mainGraph=insertGraph(sData->mainGraph, id1, id2, current_version, &indexOutSize, &indexInSize);
	}
	printNodeIndex(sData->mainGraph->indexOut, indexOutSize, sData->mainGraph->bufOut,sData->mainGraph-> bufOutInfo);
	printf("-------------------------------------------------\n");
	printNodeIndex(sData->mainGraph->indexIn, indexInSize, sData->mainGraph->bufIn, sData->mainGraph->bufInInfo);
	printf("_________________________________________________\n");
	srand(time(NULL));
	scc=estimateStronglyConnectedComponents(sData, 0, max+1);	// tarjan's algorithm to find scc
	sData->scc=scc;
	printf("we are expecting the number of components to be 2\n");
	printf("number of strongly connected components=%d\n",scc->components_count);
	printf("#################################################\n");
	SCC_Print(scc);
	
	destroyGraph(sData->mainGraph);
	free(sData);
		
}

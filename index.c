#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"

NodeIndex* createNodeIndex(NodeIndex *index, ptr size) {//init index
  ptr i=0;

	index = malloc(size*sizeof(NodeIndex));
	for (i; i<size; i++){
		index[i].head=-1;
		index[i].tail=-1;
		index[i].pos=0;
	}
	return index;
}

NodeIndex* insertNodeIndex(NodeIndex *index, ptr node, ptr *sizeOfIndex, ptr *available) {//insert node in the index
	ptr i, size = 0;

	if (node>=(*sizeOfIndex)) {//if id of the node to be inserted is greater than the size of the node increase the size
		if(node>2*(*sizeOfIndex))//if node's id is >2*size of index then allocate space equal to 2*id
			size=2*node;
		else
			size=2*(*sizeOfIndex);//double the size of the index
	    index=(NodeIndex*) realloc(index, size*(sizeof(NodeIndex)));
		for (i=(*sizeOfIndex); i<size; i++){//init the new allocated stuff
			index[i].head=-1;
			index[i].tail=-1;
			index[i].pos=0;
		}
		(*sizeOfIndex)=size;
	}
	
	if (index[node].head==-1) {//init new node's head and tail
		index[node].head=*available;
		index[node].tail=*available;
		(*available)++;
	}
	return index;
}

ptr getListHead(NodeIndex *index, ptr node, ptr sizeOfIndex) {//return the head of a node
	if (node>=sizeOfIndex)
		return -2;
	return index[node].head;
}

ptr getListTail(NodeIndex* index, ptr node,  ptr sizeOfIndex) {//return the tail of a node
  if (node >= sizeOfIndex)
		return -2;
	return index[node].tail;
}

void printNodeIndex(NodeIndex *index, ptr sizeOfIndex, ListNode* buf, BufferInfo* bufInfo) {//print the index
	ptr i=0;

	for (i; i<sizeOfIndex; i++) {
		if (index[i].head != -1) {
			printf("Position: %d, head: %d, tail: %d, pos %d\n", i,index[i].head,index[i].tail,index[i].pos);
			printBuffer(buf, bufInfo, index[i].head, index[i].tail, index[i].pos);
		}
	}
}

void destroyNodeIndex(NodeIndex *index) {//free dynami data
	if (index != NULL)
		free(index);
}

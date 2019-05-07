#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"


BufferInfo* createBufferInfo(BufferInfo* info, ptr bufsize) {//create bufInfo struct
	info = malloc(sizeof(BufferInfo));
	info->size=bufsize;
	info->available=0;
 	return info;
}

ListNode* createBuffer(ListNode* buf, ptr bufsize) {//create an array of struct Listnodes to keep its node neighbors
	ptr i, j;
	buf=malloc(bufsize*sizeof(ListNode));
	for(i=0; i<bufsize; i++)
		buf[i].nextListNode=-1;
 	return buf;
}

ListNode* doubleArray(ListNode *buffer, BufferInfo* bufInfo) { // incease the size of array if an id is greater than the size ofthe array make the array size 2*id
	ptr i,j;
	buffer=(ListNode *) realloc(buffer, (2*bufInfo->size)*(sizeof(ListNode)));
	for(i=bufInfo->size; i<(2*bufInfo->size); i++)
		buffer[i].nextListNode=-1;
	bufInfo->size=2*bufInfo->size;
	return buffer;
}

int neighbors(ListNode* buffer,ptr pos, ptr head, ptr tail, ptr node, ptr neighbor){// check if two ids are neighbors
	int i;
	while(head!=tail) {
		for(i=0; i<NEIGHBORS; i++) {
			if(buffer[head].neighbor[i]==neighbor)
				return 1;
		}
		head=buffer[head].nextListNode;
	}
	for(i=0; i<pos; i++) {
		if(buffer[tail].neighbor[i]==neighbor)
			return 1;
	}
	return 0;
}

ListNode* allocNewNode(ListNode* buffer, BufferInfo* bufInfo, ptr *pos, ptr *tail, ptr node, ptr neighbor, uint32_t versioning) {//insert a new neighbor for node
	uint32_t size;

	if((*tail)==bufInfo->size) {//if there isn't an empty listnode => realloc
		buffer=doubleArray(buffer, bufInfo);
		buffer[*tail].neighbor[(*pos)]=neighbor;
		buffer[*tail].edgeProperty[(*pos)++]=versioning;
		return buffer;
	}
	if ((*pos)==NEIGHBORS) {	// end of listnode
		if (bufInfo->available==bufInfo->size){	// full buffer => realloc
			buffer=doubleArray(buffer, bufInfo);
		}
		buffer[*tail].nextListNode=bufInfo->available;
		buffer[bufInfo->available].neighbor[0]=neighbor;
		buffer[bufInfo->available].edgeProperty[0]=versioning;
		(*tail)=bufInfo->available++;
		(*pos)=1;
	}
	else{			// just insert neighbor in the first anailable position of last listnode
		buffer[*tail].neighbor[*pos]=neighbor;
		buffer[*tail].edgeProperty[(*pos)++]=versioning;
		//(*pos)++;
	}
	return buffer;
}

ListNode* getListNode(ListNode* buf, ptr pos){//return the listnode pos
	return &buf[pos];
}

void destroyBuffer(ListNode* buf, BufferInfo* bufInfo) {//free dynamic data
	free(buf);
	free(bufInfo);
}

void printBuffer(ListNode* buf, BufferInfo* bufInfo, ptr head, ptr tail, ptr pos) {//print buffer
	ptr i=0;

	if(head==-1) return;
		printf("Neighbors:\n");
	while(head != tail) {
		printf("listnode %d\n", head);
		for(i=0; i<NEIGHBORS; i++)
			printf("%d\n", buf[head].neighbor[i]);
		head = buf[head].nextListNode;
	}
	printf("listnode %d\n", tail);
	for(i=0; i<pos; i++)
		printf("%d\n", buf[tail].neighbor[i]);
}

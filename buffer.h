#ifndef BUFFER_H
#define BUFFER_H

#include "hash.h"
#include "define.h"
#include <stdint.h>

typedef uint32_t ptr;

typedef struct ListNode{
	ptr neighbor[NEIGHBORS];			//the ids of neighbor nodes
	ptr edgeProperty[NEIGHBORS];		//property for each edge
	ptr nextListNode;						//offset of the next listnode
} ListNode;

typedef struct BufferInfo{
	ptr size;								//size of buffer
	ptr available;							//first available node
} BufferInfo;

BufferInfo* createBufferInfo(BufferInfo* info, ptr bufsize);//create and init a bufferInfo struct
ListNode* createBuffer(ListNode* buf, ptr bufsize);//create and init buffer struct
ListNode* allocNewNode(ListNode* buf, BufferInfo* bufInfo, ptr *pos, ptr *tail, ptr node, ptr neighbor, uint32_t query_version);//insert new neighbor
ListNode* doubleArray(ListNode *buf, BufferInfo* bufInfo);//increase buffer
ListNode* getListNode(ListNode* buf, ptr pos);//return the listnode pos
void printBuffer(ListNode* buf, BufferInfo* bufInfo, ptr head, ptr tail, ptr pos);//print buffer
void destroyBuffer(ListNode* buf, BufferInfo* bufInfo);//free dynamic data
int neighbors(ListNode* buf,ptr pos, ptr head, ptr tail, ptr node, ptr neighbor);//check if 2 ids are neighbors


#endif

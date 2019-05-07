#ifndef INDEX_H
#define INDEX_H

#include "buffer.h"
#include <stdint.h>

typedef uint32_t ptr;

typedef struct NodeIndex {
	ptr head;		// first listnode with neighbors
	ptr tail;		// last listnode with neighbors
	ptr pos;			// first available position of last listnode
} NodeIndex;

NodeIndex* createNodeIndex(NodeIndex *index, ptr size);//create an index
NodeIndex* insertNodeIndex(NodeIndex *index, ptr node, ptr *sizeOfIndex, ptr *available);//insert a new node in the index
ptr getListHead(NodeIndex* index, ptr node,  ptr sizeOfIndex);//get head of a node
ptr getListTail(NodeIndex* index, ptr node,  ptr sizeOfIndex);//get tail of a node
void printNodeIndex(NodeIndex* index, ptr sizeOfIndex, ListNode* buf, BufferInfo* bufInfo);//print the index
void destroyNodeIndex(NodeIndex* index);//free dynamic data

#endif

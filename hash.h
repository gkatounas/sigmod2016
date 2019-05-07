#ifndef HASH_H
#define HASH_H

#include <stdint.h>
#include "define.h"

typedef uint32_t ptr;

extern ptr sizeOfHash, sizeOfIndexOut;

typedef struct HashEntry{					// Struct for each cell of table of hash tables
  ptr* array[BUCKETSIZE];					// Hash table of each node
  ptr bucketSize[BUCKETSIZE];				// Max size of each bucket of node
  ptr bucketNum[BUCKETSIZE];				// Current size of each bucket
} HashEntry;

HashEntry *hash;								// A table with size equal to number of nodes which contains one hash table for each node

void createHashTable();
void insertHashTable(ptr node, ptr neighbor);
void doubleHashTable(ptr node);
int isNeighbor(ptr node, ptr neighbor);
void printHashTable(HashEntry* hash);
void destroyHashTable(HashEntry* hash);

#endif

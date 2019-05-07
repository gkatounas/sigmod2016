#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

void createHashTable(){							// Memory alloocation and initialization
	int i=0, j=0, k=0;

	hash = malloc(sizeOfHash*sizeof(HashEntry));
	for (i=0; i<sizeOfHash; i++){						
		for(j=0; j<BUCKETSIZE; j++){					
			hash[i].array[j] = malloc(BUCKET_NUM*sizeof(ptr));
			hash[i].bucketSize[j] = BUCKET_NUM;			
			hash[i].bucketNum[j] = 0;
		}
	}
}

void insertHashTable(ptr node, ptr neighbor){
	ptr i, j, pos, size;

	pos = neighbor%BUCKETSIZE;							// Find position of new element
	for(i=0; i<hash[node].bucketNum[pos]; i++){	// Check if neighbour already exists in another bucket
		if(hash[node].array[pos][i] == neighbor)	// In this case just return
			return;
	}
	if (hash[node].bucketSize[pos] == hash[node].bucketNum[pos]){   // Bucket is full => realloc bucket
		hash[node].array[pos] = (ptr *) realloc(hash[node].array[pos], (2*hash[node].bucketSize[pos])*(sizeof(ptr)));
		hash[node].array[pos][hash[node].bucketNum[pos]++] = neighbor;// Insert neighbour in the correct position
		hash[node].bucketSize[pos] = 2*hash[node].bucketSize[pos];	  // Double the size of the bucket
	}
	else{
		hash[node].array[pos][hash[node].bucketNum[pos]++]=neighbor;	// Just insert
	}
}

void doubleHashTable(ptr node){							// Increase size of hash table (sizeOfIndexOut) 
	ptr i, j;
	
   hash = (HashEntry*) realloc(hash, sizeOfIndexOut*(sizeof(HashEntry)));
	for (i=sizeOfHash; i<sizeOfIndexOut; i++){
		for(j=0; j<BUCKETSIZE; j++){
			hash[i].array[j] = malloc(BUCKET_NUM*sizeof(ptr));
			hash[i].bucketSize[j] = BUCKET_NUM;
			hash[i].bucketNum[j] = 0;
		}
	}
	sizeOfHash = sizeOfIndexOut;
}

int isNeighbor(ptr node, ptr neighbor){				// Check if two nodes are neighbours
	int pos, i;
	
	if(node >= sizeOfHash){
		doubleHashTable(node);
		return 0;
	}
	pos = neighbor%BUCKETSIZE;								// Find the correct position
	for (i=0; i<hash[node].bucketNum[pos]; i++){		// Check all the buckets
		if (hash[node].array[pos][i] == neighbor)
			return 1;
	}
	return 0;
}

void printHashTable(HashEntry* hash){
	int i, j, k;

	for (i=0; i<sizeOfHash; i++){
		printf("Node: %d\n", i);
		for(j=0; j<BUCKETSIZE; j++){
			printf("  Bucket %d \n   ", j);
			for (k=0; k<hash[i].bucketNum[j]; k++){
				printf("|%d", hash[i].array[j][k]);
			}
			printf("| \n");
		}
	}
}

void destroyHashTable(HashEntry* hash){
	int i=0, j=0;

	for (i=0; i<sizeOfHash; i++)
		for(j=0; j<BUCKETSIZE; j++)
		  free(hash[i].array[j]);
	free(hash);
}

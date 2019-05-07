#ifndef NODE_H
#define NODE_H

#include <stdint.h>
typedef uint32_t ptr;

typedef struct Node{	// The struct that Tarzan algorithm uses
  ptr index;
  ptr lowlink;
} Node;


#endif

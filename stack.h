#ifndef STACK_H
#define STACK_H

#include <stdint.h>
#include "define.h"

typedef struct Stack {									// Struct for a stack
	uint32_t *data;										// Array for the nodes in stack
	int counter;											// Size that counts the elements in the stack
	int size;												// The maximum size of the stack
}Stack;

Stack *stackWCC;											// Stack used in WCC

Stack* initStack(Stack *stack, uint32_t max);	// Create and init the stack
Stack* pushStack(Stack *stack, uint32_t elem);	// Push an element in the stack
Stack* popStack(Stack *stack);						// Pop the top element of the stack
uint32_t topStack(Stack *stack);						// Return the top element of the stack
int isEmpty(Stack *stack);								// Check if stack is empty or not
void printStack(Stack *stack);						// Print the content of the stack
void freeStack(Stack *stack);							// Free the allocated memory of struct stack

#endif

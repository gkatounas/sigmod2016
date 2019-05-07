#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

Stack* initStack(Stack *stack, uint32_t max) {				// Create and init a stack
	stack=malloc(sizeof(Stack));									// Allocate memory for the struct stack
	stack->data=malloc(max*sizeof(uint32_t));					// and for the array in this struct with the size max (maximum id of node)
	stack->counter=0;													// Initialize the size of struct with 0 because it's still empty
	stack->size=max;													// The max size of stack
	return stack;
}

Stack* popStack(Stack *stack) {									// Pop an element from the stack
	if (!isEmpty(stack)) 											// If stack is not empty
		stack->counter--;												// decrease the counter of stack to show the next top element
	return stack;
}

Stack* pushStack(Stack *stack, uint32_t elem) {				// Insert an element in stack
	if (stack->counter==stack->size) {							// If the number of elements in stack is equal to the maximum size
		stack->data=(uint32_t*)realloc(stack->data, 2*stack->size*sizeof(uint32_t));
		stack->size=2*stack->size;									// then double the stack and save the new size
	}
	stack->data[stack->counter++]=elem;							// Save the element in the next available position and increase the counter
	return stack;
}

uint32_t topStack(Stack *stack) {								// Return the top element of the stack
	if (isEmpty(stack)) {											// If stack is empty the return -1
		return -1;
	}
	return stack->data[stack->counter-1];						// else return the element in position size-1
}

int isEmpty(Stack *stack) {										// Check if stack is empty
	if (stack->counter == 0)  return 1;								// checking the size, if it's empty then return 1
	return 0;															// else 0
}

void printStack(Stack *stack) {									// Print the content of stack
	int i;
	for (i=0; i<stack->counter; i++)								// From 0 to size of stack
		printf("Pos: %d Elem: %d\n", i, stack->data[i]);	// print the position in stack and the element in this stack
}

void freeStack(Stack *stack) {									// Free the allocated memory of the stack
	free(stack->data);												// Firstly the array data in the stack
	free(stack);														// and after the whole stack
}

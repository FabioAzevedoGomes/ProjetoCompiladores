/**
 * Methods related to the stack data structure
 * This stack will be used for storing multiple Symbol Tables, each 
 * related to a specific static scope in the code
 * 
 * Here a global stack is also defined, for access by other methods
 */
#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "symbol_table.h"

// STACK GLOBAL VARIABLES

stack_t *stack;      // Global stack for storing data

// STACK METHODS

/**
 * @brief Initializes the global stack, creating the first (global) symbol table
 */
void stack_init();

/**
 * @brief Pushes DATA to stack top
 * @param data Data to be pushed to the stack top
 */
void push(void *data);

/**
 * @brief Pops the stack top
 * @returns Data present at the top of the stack
 */
void *pop();

#endif
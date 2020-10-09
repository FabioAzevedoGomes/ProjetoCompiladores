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

#include "symbol_table.h"

// STACK DATA TYPES

/**
 * @brief Defines an entry in the stack 
 */
typedef struct stack_entry
{
    void *data;              // Data associated with this entry
    struct stack_entry *bot; // Pointer to the stack entry under this one

} entry_t;

/**
 * @brief Defines the handler for a stack data structure
 */
typedef struct stack_handle
{
    int size;     // Amount of entries stored in the stack
    entry_t *top; // Pointer to the top of the stack

} stack_t;

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
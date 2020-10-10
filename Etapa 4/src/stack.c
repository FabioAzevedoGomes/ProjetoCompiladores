#include "stack.h"

void stack_init()
{
    // Create symbol table for global scope
    symbol_table_t *global_scope = create_symbol_table();

    // Create stack entry
    entry_t *stack_entry = (entry_t *)malloc(sizeof(entry_t));

    // Create a new stack with a single entry
    stack = (stack_t *)malloc(sizeof(stack_t));
    stack->size = 1;
    stack->top = stack_entry;

    // Fill the entry with the first symbol table (global)
    stack->top->data = (void *)global_scope;
    stack->top->bot = NULL;
}

void push(void *data)
{
    // Creates a new entry to be pushed to the stack, placing it above current top
    entry_t *new_entry = (entry_t *)malloc(sizeof(entry_t));
    new_entry->data = data;
    new_entry->bot = stack->top;

    // Update stack top
    stack->top = new_entry;

    // Update stack size
    stack->size++;
}

void *pop()
{
    void *data = NULL;   // Pointer for the retrieved data
    entry_t *aux = NULL; // Auxiliary pointer for stack updating

    // If there are entries in the stack
    if (stack->size > 0)
    {
        // Get data from the entry
        aux = stack->top;
        data = aux->data;

        // Update the stack top
        stack->top = aux->bot;

        // Free the entry memory
        free(aux);

        // Updates stack size
        stack->size--;

        // If that was the last scope (global)
        if (stack->size == 0)
        {
            // Free stack handler
            free(stack);
            stack = NULL;

        }
    }

    // Return popped data
    return data;
}
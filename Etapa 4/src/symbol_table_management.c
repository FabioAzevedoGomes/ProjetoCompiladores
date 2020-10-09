#include "symbol_table_management.h"

#include <stdio.h>
#include <stdlib.h>

extern stack_t *stack;

void init()
{
    // Initializes the stack and global symbol table
    stack_init();

    // Signals initialize
    initialized = 1;
}

void enter_scope()
{
    if (!initialized)
        init();

    // Creates a new symbol table
    symbol_table_t *st = create_symbol_table();

    // Pushes it to the stack
    push((void *)st);
}

void leave_scope()
{
    if (!initialized)
        init();

    // Pop symbol table from the stack
    symbol_table_t *st = (symbol_table_t *)pop();

    // Free symbol table memory
    free_symbol_table(st);
}

error_t *create_function_id(lexical_value_t *lv, LanguageType type, symbol_t *args, int register_args)
{
    if (!initialized)
        init();

    error_t *status = NULL;    // Status of the current operation
    symbol_t *symbol = NULL;   // Symbol being created
    symbol_t *args_aux = args; // Auxiliary pointer for counting function arguments
    int arg_count = 0;

    // Count amount of arguments in the function
    while (args_aux != NULL)
    {
        arg_count++;
        args_aux = args_aux->args;
    }

    // Create a symbol for the function identifier
    symbol = create_symbol(lv, type, KIND_FUNCTION, 1, arg_count, args);

    // Try to insert that symbol in the symbol table
    status = insert_symbol((symbol_table_t *)(stack->top->data), symbol);

    // If no error occured and arguments should be registered as well
    if (status == NULL && register_args)
    {
        // Get first argument
        args_aux = args;

        // Iterate arguments trying to insert in the symbol table
        while (args_aux != NULL && status == NULL)
        {
            // Try to insert argument
            status = insert_symbol((symbol_table_t *)(stack->top->data), args_aux);

            // Get next argument
            args_aux = args_aux->args;
        }
    }

    // Return operation status
    return status;
}

error_t *create_variable_id(lexical_value_t *lv, LanguageType type, SymbolKind kind, int amount)
{
    if (!initialized)
        init();

    error_t *status = NULL;  // Status of the current operation
    symbol_t *symbol = NULL; // Symbol being created

    // Try to create a variable symbol
    symbol = create_symbol(lv, type, kind, amount, 0, NULL);

    // Try to add that symbol to the current scope
    status = insert_symbol((symbol_table_t *)(stack->top->data), symbol);

    // Return operation status
    return status;
}

error_t *find_id(char *key, int global)
{
    if (!initialized)
        init();

    symbol_t *symbol = NULL;                              // Symbol being retrieved
    error_t *status = (error_t *)malloc(sizeof(error_t)); // Status of the current operation
    status->error_type = -1;
    entry_t *entry; // A data entry in the stack

    // Try to get the symbol in the local scope
    if ((symbol = retrieve_symbol((symbol_table_t *)(stack->top->data), key)) != NULL)
    {
        // If symbol was found, fill status with the symbol data
        status->error_type = 0;
        status->data1 = (void *)symbol;
    }
    else
    {
        // If symbol was not found but should be searched for in the global scope
        if (global)
        {
            // Get reference to the second scope
            entry = stack->top->bot;

            // Iterate through scopes searching for the symbol
            for (int i = 1; i < stack->size && (status->error_type == -1); i++, entry = entry->bot)
            {
                // If symbol was found
                if ((symbol = retrieve_symbol((symbol_table_t *)(entry->data), key)) != NULL)
                {
                    // Update status
                    status->error_type = 0;
                    status->data1 = (void *)symbol;
                }
            }
        }
        // If symbol was not found
        if (status->error_type == -1)
        {
            // Fill status with the error data
            status->error_type = ERR_UNDECLARED;
            status->data1 = (void *)key;
        }
    }

    // Return operation status
    return status;
}

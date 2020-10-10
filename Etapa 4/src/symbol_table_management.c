#include "symbol_table_management.h"

#include <stdio.h>
#include <stdlib.h>

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
    if (st != NULL)
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

st_entry_t *make_symbol_entry(lexical_value_t *lexval, int count, SymbolKind kind)
{
    if (!initialized)
        init();

    // Create a new symbol with a To-Be-Assigned type
    symbol_t *new_symbol = create_symbol(lexval, TYPE_TBA, kind, count, 0, NULL);

    // Create an entry for this symbol
    st_entry_t *new_entry = (st_entry_t *)malloc(sizeof(st_entry_t));
    new_entry->data = new_symbol;
    new_entry->next = NULL;

    return new_entry;
}

st_entry_t *make_symbol_list(st_entry_t *symbol_entry, st_entry_t *list)
{
    if (!initialized)
        init();

    st_entry_t *aux = NULL; // Auxiliary pointer to traverse the list

    if (list != NULL)
    {
        // Get reference to the second element in the list
        aux = list;

        // Traverse list
        while (aux->next != NULL)
        {
            aux = aux->next;
        }

        // Insert new symbol entry at the end of the list
        aux->next = symbol_entry;

        // Return the list head pointer
        return list;
    }
    else
    {
        // If list does not exist, symbol is the new head of the list
        return symbol_entry;
    }
}

void declare_symbol_list(st_entry_t *list, LanguageType type)
{
    if (!initialized)
        init();

    st_entry_t *aux = NULL; // Auxiliary pointer for traversing the list
    error_t *status = NULL; // Operation status

    if (list != NULL)
    {
        // Travese the entire list
        for (aux = list; aux != NULL && status == NULL;)
        {
            // Try to insert symbol
            if ((status = insert_symbol(((entry_t *)(stack->top))->data, aux->data)) == NULL)
            {
                // If insertion went ok

                // Swap pointers
                list = aux;
                aux = aux->next;

                // Free memory used for the entry in the list
                free(list);
            }
            else
            {
                // If there was an error during insertion

                // Print error information
                print_error(status);

                // TODO Maybe here stop execution? Not sure how this will work yet
            }
        }
    }
}

void check_init_types(node_t *vars, LanguageType type)
{
    LanguageType new_type; // New type being assigned to the node

    node_t *aux = NULL;     // Auxiliary pointer for traversing the list
    error_t *status = NULL; // Operation status

    node_t *identifier = NULL;  // Pointer to the node of the initialized variable
    node_t *initializer = NULL; // Pointer to the node of the initializing value

    if (vars != NULL)
    {
        // Get reference to next variable (3rd child)
        //    '<='     id     init_val  next_var
        aux = vars->children->brothers->brothers;

        do
        {
            // Update pointers
            identifier = vars->children;
            initializer = vars->children->brothers;

            // Update identifier node and initialization node type
            vars->type = type;
            identifier->type = type;

            // If identifier is being initialized with another identifier
            if (initializer->lexval->category == CAT_IDENTIFIER)
            {
                // Check if other identifier exists
                status = find_id(initializer->lexval->value.name, 1);

                // If symbol does not exist
                if (status->error_type != 0)
                {
                    print_error(status); // TODO Maybe stop execution here, not sure
                }
                else
                {
                    // If symbol existed, update initializer node type
                    initializer->type = ((symbol_t *)(status->data1))->type;

                    // Free error status
                    free(status);
                    status = NULL;
                }
            }

            // Check if types are compatible
            if ((new_type = infer_type(identifier->type, initializer->type)) != -1)
            {
                // Update identifier and initialization node types
                identifier->type = new_type;
                vars->type = new_type;

                // Get reference to the symbol
                status = find_id(identifier->lexval->value.name, 1);

                // Check return status
                if (status->error_type == 0)
                {
                    // Update the symbol type in the stack
                    ((symbol_t *)(status->data1))->type = new_type;
                }
                else
                {
                    // This should NEVER happen, as symbols are declared before this executes
                    print_error(status);

                }

                free(status);
            }
            else
            {
                // Create wrong type error
                status = create_error(ERR_WRONG_TYPE);
                status->data1 = (void *)identifier;  // Variable being initialized
                status->data1 = (void *)initializer; // Value used in initializtion

                print_error(status);

                // TODO Stop execution here? not sure
            }

            // Move to next node
            vars = aux;

            if (aux != NULL)
                aux = aux->children->brothers->brothers;

        } while (vars != NULL);
    }
}
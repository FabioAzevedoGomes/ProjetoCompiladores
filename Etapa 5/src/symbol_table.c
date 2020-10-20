#include "symbol_table.h"

symbol_table_t *create_symbol_table()
{
    // Allocate memory for a new (empty) symbol table
    symbol_table_t *new_symbol_table = (symbol_table_t *)malloc(sizeof(symbol_table_t));
    new_symbol_table->size = 0;
    new_symbol_table->first = NULL;

    return new_symbol_table;
}

symbol_t *create_symbol(lexical_value_t *lv, LanguageType type, SymbolKind kind, int amount, int arg_count, st_entry_t *args)
{
    // Copy data from lexical value to a new struct
    lexical_value_t *symbol_lexval = (lexical_value_t *)malloc(sizeof(lexical_value_t));
    symbol_lexval->category = lv->category;

    char *new_name = NULL;

    if (lv->category == CAT_LITERAL)
    {
        if (type == TYPE_STRING)
        {
            new_name = strdup(lv->value.string);
            symbol_lexval->value.string = new_name;
        }
        else
            symbol_lexval->value = lv->value;
    }
    else
    {
        new_name = strdup(lv->value.name);
        symbol_lexval->value.name = new_name;
    }

    symbol_lexval->line = lv->line;

    // Create a new symbol data structure
    symbol_t *symbol = (symbol_t *)malloc(sizeof(symbol_t));
    symbol->key = get_symbol_name(lv, type);
    symbol->count = amount;
    symbol->data = symbol_lexval;
    symbol->kind = kind;
    symbol->size = amount * type_size(type);
    symbol->type = type;
    symbol->declaration_line = symbol_lexval->line;
    symbol->args = args;
    symbol->argument_count = arg_count;

    return symbol;
}

error_t *insert_symbol(symbol_table_t *st, symbol_t *symbol)
{
    error_t *status = NULL; // Operation status

    // Entry for the new symbol
    st_entry_t *new_entry = (st_entry_t *)malloc(sizeof(st_entry_t));
    new_entry->next = NULL;
    new_entry->data = (void *)symbol;

    st_entry_t *entry = NULL;     // An entry in the symbol table
    st_entry_t *entry_aux = NULL; // Auxiliary pointer for the symbol table

    // Only inser symbols in tables that exist
    if (st != NULL)
    {
        // Get reference to first symbol in the list
        entry = st->first;
        entry_aux = entry;

        if (entry != NULL)
        {
            do
            {
                // If the symbol exists
                if (!strcmp(((symbol_t *)(entry->data))->key, symbol->key))
                {
                    // Create "Already declared" error
                    status = create_error(ERR_DECLARED);

                    // Include data about the error
                    status->data1 = (void *)symbol; // Current declaration
                    status->data2 = entry->data;    // Previous declaration
                }

                // Update pointer
                entry_aux = entry;
                entry = entry->next;

            } while (entry != NULL && status == NULL);

            // If reached the end without any errors
            if (status == NULL)
            {
                // Insert new symbol
                entry_aux->next = new_entry;

                // Update symbol table counter
                st->size++;
            }
        }
        else
        {
            // If there were no entries, new entry is the new first
            st->first = new_entry;

            // Update symbol table counter
            st->size++;
        }
    }

    // Return status
    return status;
}

symbol_t *retrieve_symbol(symbol_table_t *st, char *key, int f_par)
{
    symbol_t *symbol = NULL;  // Symbol being retrieved
    st_entry_t *entry = NULL; // An entry in the symbol table
    st_entry_t *aux = NULL;   // Auxiliary pointer for traversing parameters of functions

    // Only retrieve from symbol tables that exist
    if (st != NULL)
    {
        // Get reference to the first symbol
        entry = st->first;

        // While there are symbols in the symbol table and retreived symbol was not found
        while (entry != NULL && symbol == NULL)
        {
            // If the current symbol is the one currently being retrieved
            if (!strcmp(key, ((symbol_t *)(entry->data))->key))
            {
                // Update retrieved symbol
                symbol = (symbol_t *)(entry->data);
            }
            else
            {
                // If not, check if it is a function and it's parameters should be searched as well
                if (f_par == 1 && ((symbol_t *)(entry->data))->kind == KIND_FUNCTION)
                {
                    // If so, get first parameter reference
                    aux = ((symbol_t *)(entry->data))->args;

                    // Traverse parameter list
                    while (aux != NULL && symbol == NULL)
                    {
                        // If this parameter matches searched symbol
                        if (!strcmp(key, ((symbol_t *)(aux->data))->key))
                        {
                            // Update retrieved symbol
                            symbol = (symbol_t *)(aux->data);
                        }
                        else
                        {
                            // If not move on to next parameter
                            aux = aux->next;
                        }
                    }
                }
            }

            // Move on to next symbol
            entry = entry->next;
        }
    }

    // Returns the symbol
    return symbol;
}

void free_symbol_table(symbol_table_t *st)
{
    st_entry_t *entry;     // An entry in the symbol table
    st_entry_t *entry_aux; // Auxiliary pointer for iterating symbol table

    // Auxiliary pointers for iterating parameter list
    st_entry_t *param_aux;
    st_entry_t *param_aux_next;

    // Get reference to the first symbol entry
    entry = st->first;

    for (int i = 0; i < st->size; i++, entry = entry_aux)
    {
        // Get reference to the next entry
        entry_aux = entry->next;

        // Free the key information
        free(((symbol_t *)(entry->data))->key);

        // If entry is a function
        if (((symbol_t *)(entry->data))->kind == KIND_FUNCTION)
        {

            // Get reference to first param arguments
            param_aux = ((symbol_t *)(entry->data))->args;

            // Iterate param list
            while (param_aux != NULL)
            {

                // Free symbol data
                free(((symbol_t *)(param_aux->data))->key);
                free_lexical_value(((symbol_t *)(param_aux->data))->data, ((symbol_t *)(param_aux->data))->type);
                free((symbol_t *)(param_aux->data));

                // Get next parameter
                param_aux_next = param_aux->next;

                // Free entry
                free(param_aux);

                // Move to next parameter
                param_aux = param_aux_next;
            }
        }

        // Free current entry
        free_lexical_value(((symbol_t *)(entry->data))->data, ((symbol_t *)(entry->data))->type);
        free(entry->data);
        free(entry);
    }

    // And finally free the table handler
    free(st);
}

void print_symbol_table(symbol_table_t *st)
{

    st_entry_t *aux = st->first;
    st_entry_t *par_aux = NULL;

    // Iterate symbol table
    while (aux != NULL)
    {
        // Print symbol info
        print_symbol((symbol_t *)(aux->data));

        // If it is a function, print arguments as well
        if (((symbol_t *)(aux->data))->kind == KIND_FUNCTION)
        {
            printf("Arguments:\n");
            par_aux = ((symbol_t *)(aux->data))->args;

            // Print all parameters
            while (par_aux != NULL)
            {
                // Print param info
                print_symbol((symbol_t *)(par_aux->data));

                // Move to next parameter
                par_aux = par_aux->next;
            }
        }

        // Move to next entry
        aux = aux->next;
    }
}

void print_symbol(symbol_t *symbol)
{
    printf("========================\n");
    printf("Symbol key: %s\n", symbol->key);
    printf("Symbol type: %d\n", symbol->type);
    printf("Declared on line: %d\n", symbol->declaration_line);
    printf("Symbol kind: %d\n", symbol->kind);
    printf("Symbol argument count: %d\n", symbol->argument_count);
    printf("========================\n");
}

char *get_symbol_name(lexical_value_t *lexval, LanguageType type)
{
    char *name = NULL;

    if (lexval->category == CAT_LITERAL)
    {
        switch (type)
        {
        case TYPE_INT:
            name = calloc(20, 1);
            sprintf(name, "\"%d\"", lexval->value.integer);
            return name;
        case TYPE_FLOAT:
            name = calloc(20, 1);
            sprintf(name, "\"%f\"", lexval->value.floating);
            return name;
        case TYPE_STRING:
            name = calloc(strlen(lexval->value.string) + 3, 1);
            sprintf(name, "\"%s\"", lexval->value.string);
            return name;
        case TYPE_CHAR:
            name = calloc(3, 1);
            sprintf(name, "\"%c\"", lexval->value.character);
            return name;
        case TYPE_BOOL:
            name = calloc(6, 1);
            lexval->value.boolean ? sprintf(name, "true") : sprintf(name, "false");
            return name;
        default:
            return NULL;
        }
    }
    else
    {
        name = strdup(lexval->value.name);
        return name;
    }
}
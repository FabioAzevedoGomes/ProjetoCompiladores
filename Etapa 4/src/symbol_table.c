#include "symbol_table.h"

symbol_table_t *create_symbol_table()
{
    // Allocate memory for a new (empty) symbol table
    symbol_table_t *new_symbol_table = (symbol_table_t *)malloc(sizeof(symbol_table_t));
    new_symbol_table->size = 0;
    new_symbol_table->first = NULL;

    return new_symbol_table;
}

symbol_t *create_symbol(lexical_value_t *lv, LanguageType type, SymbolKind kind, int amount, int arg_count, symbol_t *args)
{

    // Create a new symbol data structure
    symbol_t *symbol = (symbol_t *)malloc(sizeof(symbol_t));
    symbol->key = lv->value.name;
    symbol->count = amount;
    symbol->data = lv;
    symbol->kind = kind;
    symbol->size = amount * type_size(type);
    symbol->type = type;
    symbol->declaration_line = lv->line;
    symbol->args = args; // TODO Maybe here some deconstruction is needed
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

            } while (entry != NULL);

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

symbol_t *retrieve_symbol(symbol_table_t *st, char *key)
{
    symbol_t *symbol = NULL;  // Symbol being retrieved
    st_entry_t *entry = NULL; // An entry in the symbol table

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
                // If not, move on to next entry
                entry = entry->next;
            }
        }
    }

    // Returns the symbol
    return symbol;
}

void free_symbol_table(symbol_table_t *st)
{
    st_entry_t *entry;     // An entry in the symbol table
    st_entry_t *entry_aux; // Auxiliary pointer for iterating symbol table

    // Get reference to the first symbol entry
    entry = st->first;

    for (int i = 0; i < st->size; i++, entry = entry_aux)
    {
        // Get reference to the next entry
        entry_aux = entry->next;

        // Free current entry // FIXME If entry is a function the arguments are not freed
        free(((symbol_t *)(entry->data))->key);
        free(entry->data);
        free(entry);
    }
}

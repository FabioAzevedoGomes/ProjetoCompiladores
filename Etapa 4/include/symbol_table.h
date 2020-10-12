/**
 * This file contains the data structures and methods related to the symbol table 
 */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stack.h"
#include "types.h"
#include "error_report.h"
#include "tree.h" // For freeing lexical value

// SYMBOL ENUMS

// Different kinds of symbol that can be present in the symbol table
typedef enum SymbolKind
{
    KIND_IDENTIFIER, // The symbol is a simple identifier
    KIND_VECTOR,     // The symbol is a vector
    KIND_FUNCTION,   // The symbol is a function
    KIND_NONE        // The symbol has no applicable nature

} SymbolKind;

// SYMBOL TABLE DATA TYPES

/**
 * @brief Defines an entry in the symbol table 
 */
typedef struct symbol_table_entry
{
    void *data;                      // Data stored in this entry
    struct symbol_table_entry *next; // Pointer to the next entry

} st_entry_t;

/**
 * @brief Defines a symbol in the language
 */
typedef struct symbol_data
{
    char *key; // Unique name for the symbol

    int declaration_line; // Line in the code where this symbol was declared

    int argument_count; // Amount of arguments received, if this symbol is a function
    st_entry_t *args;   // Pointer to the first argument (if this is a function) or next argument (if this is an argument)

    LanguageType type; // Assigned type for the symbol
    SymbolKind kind;   // The nature of the symbol (string, function, id, vector, etc.)

    int size;  // Occupied space for the symbol
    int count; // Amount of that type is being stored (For vectors and strings)

    lexical_value_t *data; // Data for this symbol

} symbol_t;

/**
 * @brief Defines the handler for a symbol table 
 */
typedef struct symbol_table_handler
{
    int size;          // Number of entries in the symbol table
    st_entry_t *first; // First entry in the symbol table

} symbol_table_t;

// SYMBOL TABLE METHODS

/**
 * @brief Creates a new symbol table
 * @returns Pointer to the created symbol table
 */
symbol_table_t *create_symbol_table();

/**
 * @brief Creates a new symbol using the provided data
 * @param lv        Lexical value of the symbol
 * @param type      Language type for the symbol
 * @param kind      Nature of the symbol (string, vector, id, fucntion, etc.)
 * @param amount    Amount of the type begin declared (for vectors and strings)
 * @param arg_count Amount of arguments received, in case this symbol is a function
 * @param args      Pointer to the first argument received by this symbol, if it is a function
 * @returns The created symbol 
 */
symbol_t *create_symbol(lexical_value_t *lv, LanguageType type, SymbolKind kind, int amount, int arg_count, st_entry_t *args);

/**
 * @brief Inserts a new symbol into a symbol table
 * @param st     The symbol table where the symbol should be inserted
 * @param symbol The symbol being inserted
 * @returns Descriptor with information about any semantic errors that ocurred, NULL otherwise
 */
error_t *insert_symbol(symbol_table_t *st, symbol_t *symbol);

/**
 * @brief Retrieves the symbol corresponding to KEY
 * @param st    Symbol table where the symbol is being retrieved from
 * @param key   The key for the symbol
 * @param f_par Whether the function parameters should also be considered
 * @returns The symbol associated with that key, NULL otherwise 
 */
symbol_t *retrieve_symbol(symbol_table_t *st, char *key, int f_par);

/**
 * Frees the memory used by a symbol table 
 */
void free_symbol_table(symbol_table_t *st);

/**
 * Prints every element fo the given symbol table 
 * @param st Symbol table
 */
void print_symbol_table(symbol_table_t *st);

/**
 * @brief Prints symbol information
 * @param symbol Symbol being printed 
 */
void print_symbol(symbol_t* symbol);

#endif
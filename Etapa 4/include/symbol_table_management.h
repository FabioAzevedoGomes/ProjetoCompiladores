/**
 * This file contains methods for orchestrating the management of the symbol table ambient
 * This includes creating new scopes, searching in local or higher scopes and creating 
 * symbol table entries for new IDs
 */
#ifndef ST_MANAGEMENT_H
#define ST_MANAGEMENT_H

#include "symbol_table.h"
#include "stack.h"

// Whether the global scope has been initialized
static int initialized = 0;

// TABLE ACCESS

/**
 * Initializes the stack and the global scope 
 */
void init();

/**
 * @brief Creates a new "scope" in the program 
 */
void enter_scope();

/**
 * @brief Leaves the current "scope" in the program to the previous one
 */
void leave_scope();

/**
 * @brief Creates an identifier for a function in the current scope
 * @param lv            Lexical value of the function identifier
 * @param type          The return type for the function
 * @param args          Pointer to the symbols for the arguments the function takes
 * @param register_args Whether to register arguments in the symbol table as well or not
 * @returns A description of the error if any occurred, NULL otherwise 
 */
error_t *create_function_id(lexical_value_t *lv, LanguageType type, symbol_t *args, int register_args);

/**
 * @brief Finds an identifier in the specified scope
 * @param key    Key for the identifier being searched for
 * @param global Whether this search should be in a local or global scope
 * @returns A description of the error, if error->type is 0 then data1 contains the symbol information
 */
error_t *find_id(char *key, int global);

// SYMBOL LIST CREATION

/**
 * @brief Creates a list of symbols that are going to be typed and declared in the current scope
 * @param lexval The lexical value of the new symbol being created
 * @param count  The amount of that symbol being created (For vectors)
 * @param kind   The kind of the symbol being created (Identifier or vector)
 * @returns Pointer to the new list with the created symbol added
 */
st_entry_t *make_symbol_entry(lexical_value_t *lexval, int count, SymbolKind kind);

/**
 * @brief Creates a list of symbols that are going to be typed and declared in the current scope
 * @param symbol_entry A symbol entry that should be typed and added to symbol table
 * @param list         A list of symbol entries
 * @returns Pointer to the new list with the created symbol added
 */
st_entry_t *make_symbol_list(st_entry_t *symbol_entry, st_entry_t *list);

/**
 * @brief "Declares" a list of symbols with the given type, performing semantic checks for each one
 * @param list A list of symbol entries that are being declared
 * @param type The type these symbols should receive
 */
void declare_symbol_list(st_entry_t *list, LanguageType type);

// LOCAL SYMBOL LIST CREATION

/**
 * @brief Checks:
 * - If initializing type is compatible with identifier type
 * - If initializing value is an identifier, if it was declared previously
 * Also updates the variable initialization nodes with the proper type
 * @param vars List of initialized variables
 * @param type The type begin assigned to the variables
 */
void check_init_types(node_t *vars, LanguageType type);

#endif
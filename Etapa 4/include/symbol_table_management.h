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
 * @brief Creates an identifier for a variable in the current scope 
 * @param lv     Lexical value for this variable identifier
 * @param type   Language type for this variable
 * @param kind   Nature of the variable being created (string, id, vector, function, etc.)
 * @param amount How many of that type is being declared (For strings and vectors)
 * @returns A description of the error if any occurred, NULL otherwise
 */
error_t *create_variable_id(lexical_value_t *lv, LanguageType type, SymbolKind kind, int amount);

/**
 * @brief Finds an identifier in the specified scope
 * @param key    Key for the identifier being searched for
 * @param global Whether this search should be in a local or global scope
 * @returns A description of the error, if error->type is 0 then data1 contains the symbol information
 */
error_t *find_id(char *key, int global);

#endif
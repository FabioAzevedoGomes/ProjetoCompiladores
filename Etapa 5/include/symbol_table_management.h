/**
 * This file contains methods for orchestrating the management of the symbol table ambient
 * This includes creating new scopes, searching in local or higher scopes and creating 
 * symbol table entries for new IDs
 */
#ifndef ST_MANAGEMENT_H
#define ST_MANAGEMENT_H

#include "symbol_table.h"
#include "stack.h"
#include "types.h"

// TABLE ACCESS

st_entry_t *current_function; // Pointer to current function being analyzed
int func_depth;               // Signals if function parameter declaration was already done

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

// LOCAL SYMBOL LIST TYPE CHECKING

/**
 * @brief Checks:
 * - If initializing type is compatible with identifier type
 * - If initializing value is an identifier, if it was declared previously
 * Also updates the variable initialization nodes with the proper type
 * @param vars List of initialized variables
 * @param type The type begin assigned to the variables
 */
void check_init_types(node_t *vars, LanguageType type);

// FUNCTION DECLARATION

/**
 * @brief Creates a symbol table entry for a function 
 * @param lexval    Lexical value of the function ID
 * @param type      Return type of the function
 * @param arg_count Number of arguments the function receives
 */
st_entry_t *make_function_entry(lexical_value_t *lexval, LanguageType type);

/**
 * @brief Creates an entry for a function parameter
 * @param param Lexical value of the parameter being used
 * @param type  Type of this parameter
 * @returns Pointer to the created symbol table entry
 */
st_entry_t *make_param_entry(lexical_value_t *param, LanguageType type);

/**
 * @brief Creates a list using a given parameter and an already existing param list
 * @param param Symbol table entry for the new parameter
 * @param list  List of parameters
 * @returns Pointer to the new parameter list
 */
st_entry_t *make_param_list(st_entry_t *param, st_entry_t *list);

/**
 * @brief Declares a function in the symbol table
 * @param function Pointer to the function being declared
 * @param params Pointer to the list of parameters of this fucntion
 * @param global Whether this is a global declaration, i.e., whether to declare params in the symbol table as well
 * @returns Pointer to the declared function, so it may be kept track of by the parser
 */
st_entry_t *declare_function(st_entry_t *function, st_entry_t *params, int global);

/**
 * @brief Declares a list of parameters to the current scope 
 * @param params List of parameters
 */
void declare_params(st_entry_t *params);

#endif
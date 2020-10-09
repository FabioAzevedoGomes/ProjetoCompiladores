/**
 * This file has definitions about the AST data structure, as well as methods for manipulating it
 */
#ifndef TREE_H
#define TREE_H

#include "types.h"
#include "symbol_table.h"

// STRUCTURES

/**
 * Defines a node from the AST
 */
typedef struct node
{
    lexical_value_t *lexval; // Node information

    Statement st_kind; // What kind of statement this node is (if, for, while, atrib, etc.)
    LanguageType type; // Node type according to the language definitions

    struct node *children; // Pointer to the this node's first child
    struct node *brothers; // Pointer to the this node's next brother
    struct node *next_cmd; // Pointer to the next command after this node's command

} node_t;

// TREE MEMORY MANAGEMENT FUNCTIONS

/**
 * @brief Exports the given AST to stdout
 * @param arvore Pointer to the root of the AST to be exported
 */
extern void exporta(void *arvore);

/**
 * @brief Frees up memory for a given AST
 * @param arvore Pointer to the root of the AST that should be removed
 */
extern void libera(void *arvore);

/**
 * @brief Frees the memory used by a lexical value pointer, given it's nature
 * @param lexval Pointer to the lexical value structure
 * @param type What it the type of this lexical value
 */
void free_lexical_value(lexical_value_t *lexval, LanguageType type);

// NODE CREATION METHODS

/**
 * @brief Creates and returns a pointer to a node using the provided lexical value 
 * @param lexval Lexical value of the token being used to create the node
 * @param type   The type of this node, TYPE_TBA if not yet known
 * @param kind   The kind of statement for this node
 * @returns Pointer to the node
 */
node_t *create_lexical_node(lexical_value_t *lexval, LanguageType type, Statement st_kind);

/**
 * @brief Inserts the second command as next_cmd of the first comand
 * @param first  First command in the list
 * @param second Second command in the list
 * @returns Pointer to the first command
 */
node_t *create_command_chain(node_t *first, node_t *second);

/**
 * @brief Inserts a child node to the given father. If father is NULL, child is returned
 * @param father Father node
 * @param child  Child node
 * @returns Pointer to the resulting father node
 */
node_t *insert_child(node_t *father, node_t *child);

// STATEMENT NODE CREATION METHODS

/**
 * @brief Creates a node for an IF statement 
 * @param condition       Node containing the condition expression
 * @param true_statement  Node containing the THEN code block
 * @param false_statement Node containing the ELSE code block (or NULL)
 * @returns Pointer to the created node
 */
node_t *create_if(node_t *condition, node_t *true_statement, node_t *false_statement);

/**
 * @brief Creates a node for a FOR statement
 * @param init_atrib Node containing initial attribution
 * @param condition  Node containing condition expression
 * @param loop_atrib Node containing loop attribution
 * @param statement  Node containing the code block
 * @returns Pointer to the created node
 */
node_t *create_for(node_t *init_atrib, node_t *condition, node_t *loop_atrib, node_t *statement);

/**
 * @brief Creates a node for a WHILE statement
 * @param condition Node containing the condition expression
 * @param statement Node containing the code block
 * @returns Pointer to the created node
 */
node_t *create_while(node_t *condition, node_t *statement);

/**
 * @brief Creates a node for a vector access operation
 * @param identifier Node containing the vector identifier
 * @param index      Node containing the indexing expression
 * @returns Pointer to the created node
 */
node_t *create_vector_access(node_t *identifier, node_t *index);

/**
 * @brief Creates a node for an INPUT statement
 * @param identifier Node containing the identifier used
 * @returns Pointer to the created node
 */
node_t *create_input(node_t *identifier);

/**
 * @brief Creates a node for an OUTPUT statement 
 * @param operand Node containing operand used 
 * @returns Pointer to the created node 
 */
node_t *create_output(node_t *operand);

/**
 * @brief Creates a node for a CONTINUE statement
 * @param line Line where the command occurred
 * @returns Pointer to the created node
 */
node_t *create_continue(int line);

/**
 * @brief Creates a node for a BREAK statement
 * @param line Line where the command occurred
 * @returns Pointer to the created node
 */
node_t *create_break(int line);
/**
 * @brief Creates a node for a RETURN statement
 * @param retval Node containing value being returned
 * @returns Pointer to the created node 
 */
node_t *create_return(node_t *retval);

/**
 * @brief Creates a node for a SHIFT statement
 * @param identifier Node containing ientifier being shifted
 * @param amount     Node containing amount of bits being shifted
 * @param operator   Node containing the shift operation
 * @returns Pointer to the created node
 */
node_t *create_shift(node_t *identifier, node_t *amount, node_t *operator);

/**
 * @brief Creates a node for an ATTRIBUTION statement
 * @param lval Node containing the left value of the attribution
 * @param rval Node containing the right value of the attribution
 * @param operator  Node containing the attribution operator
 * @returns Pointer to the created node
 */
node_t *create_attrib(node_t *lval, node_t *rval, node_t *operator);

/**
 * @brief Creates a node for an INITIALIZATION statement
 * @param identifier Node containing the identifier being initialized
 * @param rval       Node containing value used to initialize that variable
 * @param operator  Node containing the initialization operator
 * @returns Pointer to the created node
 */
node_t *create_init(node_t *identifier, node_t *rval, node_t *operator);

/**
 * @brief Creates a node for a UNARY OPERATION expression
 * @param operation Node containing operation being applied
 * @param operand   Node containing operand being used
 * @returns Pointer to the created node
 */
node_t *create_unop(node_t *operation, node_t *operand);

/**
 * @brief Creates a node for a BINARY OPERATION expression
 * @param operation Node containing operation being applied
 * @param lval      Node containing left value of the operation
 * @param rval      Node containing right value of the operation
 * @returns Pointer to the created node
 */
node_t *create_binop(node_t *operation, node_t *lval, node_t *rval);

/**
 * @brief Partially creates a node for a TERNOP statement. Node is completed when BINOP is created
 * @param true_statment   Node containing expression if condition is true
 * @return Pointer to the created node
 */
node_t *create_ternop(node_t *true_statement);

/**
 * @brief Creates a node for a function declaraction
 * @param identifier Node containing the function identifier
 * @param body       Node containing the code block
 * @param type       The return type for this function
 * @returns Pointer to the created node
 */
node_t *create_function_declaration(node_t *identifier, node_t *body, LanguageType type);

/**
 * @brief Creates a node for a function call
 * @param identifier Node containing the called function identifier
 * @param args       Node containing a list of arguments being passed
 */
node_t *create_function_call(node_t *identifier, node_t *args);

#endif
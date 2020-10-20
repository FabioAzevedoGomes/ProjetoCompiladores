/**
 * This file defines some of the types used throughout the entire compiler 
 */
#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>

// UNIONS

// Possible values for a token
typedef union TokenValue
{
    char *name; // Identifiers, operators and special characters

    int integer;    // Integers
    float floating; // Floats
    char *string;   // Strings
    char character; // Chars
    int boolean;    // Bools

} TokenValue;

// ENUMERATIONS

// Available categories of token
typedef enum TokenCategory
{
    CAT_SPECIAL_CHARACTER,  // Special characters such as '.' and ';'
    CAT_COMPOSITE_OPERATOR, // A composite operator, such as '<=' or '||'
    CAT_IDENTIFIER,         // An identifier, such as a variable or function name
    CAT_LITERAL,            // A literal value, such as '50' or '"foo"'
    CAT_RESERVED            // A reserved word from the language, such as 'if' and 'output'

} TokenCategory;

// Available types in the language
typedef enum LanguageType
{
    TYPE_INT,    // Language integers
    TYPE_FLOAT,  // Language floats
    TYPE_BOOL,   // Language bools
    TYPE_CHAR,   // Language chars
    TYPE_STRING, // Language strings
    TYPE_TBA,    // Unknown type, "To Be Assigned"
    TYPE_NA,     // Typing not applicable, used for nodes containing commands
    TYPE_ANY     // Any type is acceptable, for operators such as & and *

} LanguageType;

// Available statements in the language
typedef enum Statement_Kinds
{
    CMD_FUNCTION_DECLARATION, // A function declaration
    CMD_FUNCTION_CALL,        // A function call
    CMD_INIT_VARIABLE,        // A variable initialization
    CMD_ATTRIB_VARIABLE,      // A variable attribution
    CMD_IO,                   // An input/output command
    CMD_SHIFT,                // A shift command
    CMD_BREAK_CONTINUE,       // A break/continue command
    CMD_RETURN,               // A return command
    CMD_IF,                   // An if statement
    CMD_FOR,                  // A for statement
    CMD_WHILE,                // A while statement
    CMD_UNOP,                 // A unary operation
    CMD_BINOP,                // A binary operation
    CMD_TERNOP,               // A ternary operation
    CMD_OPERAND,              // An operand such as an identifier or a literal value
    CMD_VECTOR_ACCESS         // An access to a vector position

} Statement;

// Different kinds of symbol that can be present in the symbol table
typedef enum SymbolKind
{
    KIND_IDENTIFIER, // The symbol is a simple identifier
    KIND_VECTOR,     // The symbol is a vector
    KIND_FUNCTION,   // The symbol is a function
    KIND_NONE        // The symbol has no applicable nature

} SymbolKind;

// Operation codes for ILOC intermediate code language
typedef enum ILOC_Codes
{
    ILOC_NOP,
    // Arithmetics
    ILOC_ADD,
    ILOC_SUB,
    ILOC_MULT,
    ILOC_DIV,
    ILOC_ADDI,
    ILOC_SUBI,
    ILOC_RSUBI,
    ILOC_MULTI,
    ILOC_DIVI,
    ILOC_RDIVI,
    // Shifts
    ILOC_LSHIFT,
    ILOC_LSHIFTI,
    ILOC_RSHIFT,
    ILOC_RSHIFTI,
    // Loads
    ILOC_LOAD,
    ILOC_LOADI,
    ILOC_LOADAI,
    ILOC_LOADA0,
    // Stores
    ILOC_STORE,
    ILOC_STOREAI,
    ILOC_STOREAO,
    // Copy
    ILOC_I2I,
    // Compares
    ILOC_CMP_LT,
    ILOC_CMP_L3,
    ILOC_CMP_EQ,
    ILOC_CMP_GE,
    ILOC_CMP_GT,
    ILOC_CMP_NE,
    // Jumps
    ILOC_CBR,
    ILOC_JUMPI,
    ILOC_JUMP

} ILOCop;

// STRUCTURES

/**
 * @brief Defines the lexical value of a token received from the scanner
 */
typedef struct lexical_value
{
    int line;               // Line where this token appears
    TokenCategory category; // Token category
    TokenValue value;       // Token value

} lexical_value_t;

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

// STACK DATA TYPES

/**
 * @brief Defines an entry in the stack 
 */
typedef struct stack_entry
{
    void *data;              // Data associated with this entry
    struct stack_entry *bot; // Pointer to the stack entry under this one

} entry_t;

/**
 * @brief Defines the handler for a stack data structure
 */
typedef struct stack_handle
{
    int size;     // Amount of entries stored in the stack
    entry_t *top; // Pointer to the top of the stack

} stack_t;

// TREE STRUCTURES

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

// ILOC DATA TYPES

/**
 * @brief An ILOC command data structure
 */
typedef struct iloc_element
{
    ILOCop opcode; // ILOC operation code
    void *arg1;    // First argument
    void *arg2;    // Second argument
    void *arg3;    // Third argument

} iloc_t;

/**
 * @brief A member of the ILOC command list 
 */
typedef struct iloc_element_list
{
    struct iloc_element_list *next; // Next command in list
    struct iloc_element_list *prev; // Previous command in list
    iloc_t *command;                // ILOC command

} iloc_list_t;

// FUNCTIONS

/**
 * @brief Frees the memory used by a lexical value pointer, given it's nature
 * @param lexval Pointer to the lexical value structure
 * @param type What it the type of this lexical value
 */
void free_lexical_value(lexical_value_t *lexval, LanguageType type);

/**
 * @brief Returns the size for a given type
 * @param type Type whose size is to be returned
 * @returns Size for given type
 */
int type_size(LanguageType type);

/**
 * @brief Decides if given types are compatible
 * @param type1 First type
 * @param type2 Second type
 * @returns 1 if types are compatible, 0 otherwise 
 */
int compatible_types(LanguageType type1, LanguageType type2);

/**
 * @brief Infers the resulting type from the combination of 2 types
 * @param type1 First type
 * @param type2 Second type
 * @returns Resulting type, -1 if incompatible
 */
LanguageType infer_type(LanguageType type1, LanguageType type2);

/**
 * @brief Returns the name of the type as a string
 * @param type The type
 */
char *type_name(LanguageType type);

#endif

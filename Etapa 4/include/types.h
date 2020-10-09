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
    TYPE_NA      // Typing not applicable, used for nodes containing commands

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

// FUNCTIONS

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

#endif

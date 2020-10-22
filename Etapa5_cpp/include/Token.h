/**
 * This file contains the Token class, created by the scanner when identifying
 * tokens in the input program. Contains methods for creating and freeing the lexical
 * value associated with this token.
 * 
 * Author: Fábio de Azevedo Gomes
 */
#ifndef TOKEN_H
#define TOKEN_H

#include <string.h>
#include <sstream>

#include "Type.h"

// Union containing the lexical value for the token
typedef union
{
    char *name; // For identifiers, operators, special characters and strings

    int integer;    // Ints
    float floating; // Floats
    char character; // Chars
    bool boolean;   //Bool

} Value;

class Token
{

private:
    int line;          // Line where this token occurred
    Category category; // Token category
    Value value;       // Token value
    int field;         // Field being used in the union

    char *name; // Token name as a string

public:
    // CONSTRUCTOR AND DESTRUCTOR

    /**
     * @brief Constucts a token from it's describing data
     * @param line_      Line where token was captured
     * @param category_  Token category
     * @param value_     Lexical value for the token
     * @param filed_     Field being used in the union
     */
    Token(int line_, Category category_, Value value_, int field_);

    /**
     * @brief Copy constructor
     * @param token Another token to copy from 
     */
    Token(Token &token);

    /**
     * @brief Frees the memory used by this token 
     */
    ~Token();

    /**
     * @brief Generates a name for this token based on it's value
     */
    char *generateName();

    // GETTERS

    /**
     * @brief Retruns token category 
     */
    Category getCategory();

    /**
     * @brief Returns token value 
     */
    Value getValue();

    /**
     * @brief Returns token line of occurrence 
     */
    int getLine();

    /**
     * @brief Returns token name as a string 
     */
    char *getName();

    /**
     * @brief Returns field used by this token 
     */
    int getField();
};

#endif
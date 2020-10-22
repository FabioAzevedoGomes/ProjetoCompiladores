/**
 * This file contains the definition of multiple exception types that are used throughout
 * the compiler semantic checks for error reporting.
 * 
 * Author: Fábio de Azevedo Gomes
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

#include "Symbol.h"
#include "Node.h"
#include "errors.h"

/**
 * @brief Returns current line in input file/stream
 * Defined in scanner.l 
 */
extern int get_line_number();

/**
 * @brief This exception is thrown when attempting to insert a symbol that 
 * already exists into a symbol table 
 */
class DeclaredException : public std::exception
{
private:
    Symbol *inserted; // Symbol being inserted
    Symbol *existing; // Symbol that already existed with the same key

public:
    /**
     * @brief Exception constructor
     * @param inserted_ Symbol being inserted
     * @param existed_  Symbol with same key that already existed
     */
    DeclaredException(Symbol *inserted_, Symbol *existed_);

    /**
     * @brief Outputs exception information to a readable format
     * @returns Exception explanation 
     */
    std::string what();
};

/**
 * @brief This exception is thrown when attempting to retrieve a symbol that
 * does not exist from the symbol table 
 */
class UndeclaredException : public std::exception
{
private:
    std::string key; // Key for the symbol being searched

public:
    /**
     * @brief Exception constructor 
     * @param key_ Key for the symbolbeign searched
     */
    UndeclaredException(std::string key_);

    /**
     * @brief Outputs exception information to a readable format
     * @returns Exception explanation 
     */
    std::string what();
};

/**
 * @brief This exception is thrown when attempting to assign a value to a 
 * variable with incompatible typing 
 */
class WrongTypeException : public std::exception
{
private:
    Node *usage; // Node containing incorrect attribution

public:
    /**
     * @brief Exception constructor 
     * @param usage_  Node containing incorrect attribution
     */
    WrongTypeException(Node *usage_);

    /**
     * @brief Outputs exception information to a readable format
     * @returns Exception explanation 
     */
    std::string what();
};

/**
 * @brief This exception is thrown when attempting to use a variable, function
 * or vector identifier as any of the other 2 natures 
 */
class WrongUsageException : public std::exception
{
private:
    Symbol *symbol; // Symbol declaration
    Node *usage;    // Node containing incorrect usage
    int code;       // Error code generated

public:
    /**
     * @brief Exception constructor 
     * @param symbol_ Original declaration of the symbol
     * @param usage_  Node containing incorrect usage 
     */
    WrongUsageException(Symbol *symbol_, Node *usage_);

    /**
     * @brief Returns associated error code 
     */
    int getCode();

    /**
     * @brief Outputs exception information to a readable format
     * @returns Exception explanation 
     */
    std::string what();
};

/**
 * @brief This exception is thrown when an invalid conversion is attempted,
 * such as string or char to other types 
 */
class InvalidConversionException : public std::exception
{
private:
    Node *usage; // Node containing incorrect usage
    Type type1;  // Type attempting to be converted
    Type type2;  // Type the first is being converted to
    int code;    // Error code generated

public:
    /**
     * @brief Exception constructor
     * @param usage_ Node containing incorrect usage
     * @param type1_ Invalid type attempting to be converted
     * @param type2_ Type it's attempting to convert to
     */
    InvalidConversionException(Node *usage_, Type type1_, Type type2_);

    /**
     * @brief Returns associated error code 
     */
    int getCode();

    /**
     * @brief Outputs exception information to a readable format
     * @returns Exception explanation 
     */
    std::string what();
};

/**
 * @brief This exception is thrown when an assignment is attempted using a string
 * with insuficient size
 */
class InvalidStringSizeException : public std::exception
{
private:
    Node *usage;    // Node containing incorrect usage
    Symbol *symbol; // String symbol delcaration
    int size;       // Rval expression size

public:
    /**
     * @brief Exception constructor
     * @param usage_  Node containing incorrect usage
     * @param symbol_ Symbol information about the string id
     * @param size_   Size of expression being given to string
     */
    InvalidStringSizeException(Node *usage_, Symbol *symbol_, int size_);

    /**
     * @brief Outputs exception information to a readable format
     * @returns Exception explanation 
     */
    std::string what();
};

/**
 * @brief This exception is thrown when a wrong parameter type is passed
 * to an input, output or return command
 */
class WrongParameterException : public std::exception
{
private:
    Node *usage;   // Node containing incorrect usage
    Type expected; // Expected type
    Type given;    // Given type
    Symbol *func;  // If this was a return, function it belongs to
    int code;      // code generated for this error

public:
    /**
     * @brief Exception constructor
     * @param usage_     Node containing incorrect usage
     * @param expected_  Expected type parameter
     * @param given_     Given type in that usage
     * @param func_      If this was a return statement, what function it was at
     */
    WrongParameterException(Node *usage_, Type expected_, Type given_, Symbol *func_ = NULL);

    /**
     * @brief Returns associated error code 
     */
    int getCode();

    /**
     * @brief Outputs exception information to a readable format
     * @returns Exception explanation 
     */
    std::string what();
};

/**
 * @brief This exception is thrown when an amount higher than SHIFT_MAX is
 * passed as shift amount
 */
class WrongShiftAmountException : public std::exception
{
private:
    Node *usage;
    int amount;

public:
    /**
     * @brief Exception constructor
     * @param usage_ Node containing incorrect shift amount
     */
    WrongShiftAmountException(Node *usage_);

    /**
     * @brief Outputs exception information to a readable format
     * @returns Exception explanation 
     */
    std::string what();
};

/**
 * @brief This exception is thrown when wrong arguments are passed
 * to a function. 
 */
class WrongArgumentsException : public std::exception
{
private:
    Node *usage;  // Node containing incorrect call
    Symbol *func; // Function symbol
    int index;    // Index of incorrect argument
    int code;     // Associated error code

public:
    /**
     * @brief Exception constructor
     * @param usage_     Node containing incorrect usage
     * @param func_      Function symbol
     * @param code_      Error code for that incorrect usage
     * @param index_     If incorrect argument type, at which index it happened
     */
    WrongArgumentsException(Node *usage_, Symbol *func_, int code_, int index_ = -1);

    /**
     * @brief Returns associated error code 
     */
    int getCode();

    /**
     * @brief Outputs exception information to a readable format
     * @returns Exception explanation 
     */
    std::string what();
};

#endif

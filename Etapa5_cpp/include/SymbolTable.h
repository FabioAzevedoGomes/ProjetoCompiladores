/**
 * This class has methods and data associated with a single symbol table, usually 
 * present inside of a static scope from the input program. Symbol tables that are 
 * inside of a scope only have access to those "higher" up above it, as dictated by
 * the stack hierarchy used to manage these scopes.
 * 
 * Author: Fábio de Azevedo Gomes
 */
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "Symbol.h"
#include "Exceptions.h"

#include <unordered_map>
#include <sstream>

class SymbolTable
{

private:
    std::unordered_map<std::string, Symbol *> table; // The hash table

    Symbol *function; // Function to which this table is the at body of

    int current_address; // Current address for declared symbols
    bool global;         // Whether this is the global symbol table

public:
    /**
     * @brief Class constructor
     * @param start_address Starting address for this symbol table
     * @param global Whether this is the global symbol table, false by default 
     * @param function_ If this is the body of a function, which function it is
     */
    SymbolTable(int start_address_, bool global_ = false, Symbol *function_ = NULL);

    /**
     * @brief Class destructor 
     */
    ~SymbolTable();

    /**
     * @brief Returns a string containing this symbol table's contents
     * @returns Readable symbol table
     */
    std::string toString();

    // GETTERS

    /**
     * @brief Finds the given symbol in the table
     * @param symbol String containing the symbol key
     * @throw UndeclaredException if symbol is not found
     * @returns Reference to the symbol 
     */
    Symbol *getSymbol(std::string key);

    /**
     * @brief Returns the current address for this symbol table
     */
    int getAddress();

    /**
     * @brief Inserts the given symbol in the symbol table
     * @param symbol The symbol being inserted
     * @param key    Key used for identifying this symbol
     * @throw DeclaredException if symbol is already in table
     */
    void insertSymbol(Symbol *symbol, std::string key);

    // SETTERS

    /**
     * @brief Sets the current symbol table address to the given one
     * @param address The new memory address 
     */
    void setAddress(int address);
};

#endif
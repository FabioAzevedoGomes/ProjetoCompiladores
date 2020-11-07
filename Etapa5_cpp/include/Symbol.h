/**
 * This file contains the Symbol class, which is present in the symbol table that is created
 * for each of the input program's static scopes
 * 
 * Author: Fábio de Azevedo Gomes 
 */

#ifndef SYMBOL_H
#define SYMBOL_H

#include "Type.h"
#include "Token.h"

#include <vector>
#include <sstream>

class Symbol
{

private:
    int declare_line; // Line where this symbol was declared
    int count, size;  // Symbol count and occupied size

    bool global; // Whether this is a global symbol or not

    Type type;     // Symbol type
    Nature nature; // Symbol nature

    std::string *label;           // Label given to the first instruction in this function
    std::vector<Symbol *> params; // List of parameters if this symbol is a function

    Token *data; // Symbol token data

    int address; // Memory address for this symbol

public:
    // CONSTRUCTOR AND DESTRUCTOR

    /**
     * @brief Creates a symbol based on it's describing data
     * @param data_   Token that generated this symbol
     * @param nature_ Symbol nature
     * @param type_   Symbol type
     * @param count_  Symbol count
     * @param global_ Whether this is a global symbol (false by default)
     */
    Symbol(Token *data_, Nature nature_, Type type_, int count_, bool global_ = false);

    /**
     * @brief Copy constructor
     * @param symbol Symbol being copied
     */
    Symbol(Symbol &symbol);

    /**
     * @brief Class destructor
     */
    ~Symbol();

    /**
     * @brief Outputs symbol information to a readable format
     * @return String containing symbol description
     */
    std::string toString();

    /**
     * @brief Reconstructs the symbol declaration into an approximation
     * of the original code, used for error reporting
     */
    std::string getDeclaration();

    // UPDATERS

    /**
     * @brief Inserts a new parameter argument at the start of the symbol's parameter list
     * @param param Symbol for the new parameter
     */
    void addParameter(Symbol *param);

    /**
     * @brief Updates the symbol type
     * @param type New symbol type 
     */
    void setType(Type type);

    /**
     * @brief Sets this symbol's address in memory
     * @param address new address
     */
    void setAddress(int address);

    /**
     * @brief Updates the declaration line for this symbol, used
     * only for repeating literals 
     */
    void setLine(int line);

    /**
     * @brief Check for size compatibility between count and current size, and update if ok
     * @param count New size being given
     * @returns true is new size is valid, false otherwise
     */
    bool updateSize(int count);

    /**
     * @brief Gives a new label to this function, which points to it's code segment startint poirnt 
     * @param label The label being given
     */
    void giveLabel(std::string *label);

    // GETTERS

    Category getCategory();

    Nature getNature();

    Type getType();

    const char *getName();

    int getLine();

    std::vector<Symbol *> getParams();

    int getOccupiedSize();

    int getAddress();

    bool isGlobal();

    std::string *getLabel();
};

#endif

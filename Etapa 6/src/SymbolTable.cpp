#include "SymbolTable.h"

SymbolTable::SymbolTable(int start_address_, bool global_, Symbol *function_)
{
    this->current_address = start_address_;
    this->global = global_;
    this->function = function_;
}

SymbolTable::~SymbolTable()
{
    for (auto i = this->table.begin(); i != this->table.end(); ++i)
        delete i->second;
}

std::string SymbolTable::toString()
{
    // Output string
    std::stringstream output;

    // Add address information
    output << "Symbol table with current address at " << this->current_address << ": " << std::endl;

    // Inform about global status
    if (this->global)
    {
        output << "THIS IS THE GLOBAL SYMBOL TABLE" << std::endl;
    }

    // Iterate table
    for (auto i = this->table.begin(); i != this->table.end(); ++i)
    {
        // Print key information
        output << "Symbol with key: " << i->first << std::endl;
        output << i->second->toString() << std::endl;
    }

    // Return string
    return output.str();
}

Symbol *SymbolTable::getSymbol(std::string key)
{
    Symbol *symbol;

    try
    {
        // Get symbol
        symbol = table.at(key);
    }
    catch (const std::out_of_range &e)
    {
        // Throw exception
        throw new UndeclaredException(key);
    }

    // Return retreived symbol
    return symbol;
}

Symbol *SymbolTable::getSymbolByAddress(int address)
{
    for (auto i = this->table.begin(); i != this->table.end(); ++i)
        if (i->second->getAddress() == address && i->second->getNature() != NAT_FUNCTION)
            return i->second;

    return NULL;
}

int SymbolTable::getAddress()
{
    return this->current_address;
}

std::unordered_map<std::string, Symbol *> SymbolTable::getHashTable()
{
    return this->table;
}

void SymbolTable::insertSymbol(Symbol *symbol, std::string key)
{

    // Update symbol memory address
    symbol->setAddress(this->current_address);

    // Try to insert
    auto p = table.insert({key, symbol});

    // If not a function
    if (symbol->getNature() != NAT_FUNCTION)
    {
        // Update table address
        this->current_address += symbol->getOccupiedSize();
    }

    // If not sucessful
    if (!p.second)
    {
        // Throw exception
        throw new DeclaredException(symbol, (p.first)->second);
    }
}

void SymbolTable::setAddress(int address)
{
    this->current_address = address;
}

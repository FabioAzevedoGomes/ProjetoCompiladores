#include "Symbol.h"

Symbol::Symbol(Token *data_, Nature nature_, Type type_, int count_)
{
    // Copy token data
    Token *copy = new Token(*data_);

    this->data = copy;
    this->nature = nature_;
    this->type = type_;
    this->count = count_;
    this->declare_line = data_->getLine();
    this->size = count_ * getSize(type_);
    this->address = -1;
}

Symbol::Symbol(Symbol &symbol)
{
    Token *copy = new Token(*(symbol.data));

    this->data = copy;
    this->nature = symbol.nature;
    this->type = symbol.type;
    this->count = symbol.count;
    this->declare_line = symbol.declare_line;
    this->size = symbol.size;
    this->address = symbol.address;
}

Symbol::~Symbol()
{
    // If symbol is a function
    if (this->nature == NAT_FUNCTION)
        // Iterate parameters
        for (auto i = this->params.begin(); i != this->params.end(); ++i)
            // Free parameter
            delete *i;

    // Free lexical value
    delete (this->data);
}

std::string Symbol::toString()
{
    std::stringstream output;

    output << "+ Name: " << this->getName() << std::endl;
    output << "| Category: " << this->getCategory() << std::endl;
    output << "| Nature: " << this->getNature() << std::endl;
    output << "| Type: " << getTypeName(this->type) << std::endl;
    output << "| Declared on line: " << this->declare_line << std::endl;
    output << "| Alloc'd at: " << this->getAddress() << std::endl;
    output << "| Size: " << this->size << std::endl;
    output << "| Amount: " << this->count << std::endl;
    output << "| Paramter count: " << this->params.size() << std::endl
           << std::endl;

    // Return symbol info
    return output.str();
}

std::string Symbol::getDeclaration()
{
    std::stringstream declaration;

    // If this is an identifier
    if (this->getCategory() == CAT_IDENTIFIER)
    {
        // Insert the symbol type and name
        declaration << getTypeName(this->type) << " " << this->getName();

        switch (this->nature)
        {
        // If its a variable, do nothing else
        case NAT_IDENTIFIER:
            break;
        // If this is a vector, add size in brackets
        case NAT_VECTOR:
            declaration << "[" << this->size << "]";
            break;
        // If this is a function, add parameter list in parenthesis
        case NAT_FUNCTION:
            declaration << "(";

            // Iterate params
            for (auto i = this->params.begin(); i != this->params.end(); ++i)
            {
                // Don't add ',' on first parameter
                if (i != this->params.begin())
                    declaration << ", ";

                // Add param declaration
                declaration << (*i)->getDeclaration();
            }

            declaration << ")";
            break;
        default:
            break;
        }
    }
    return declaration.str();
}

// SETTERS

void Symbol::addParameter(Symbol *param)
{
    // Copy symbol
    Symbol *copy = new Symbol(*param);

    // Insert copy
    this->params.push_back(copy);
}

void Symbol::setType(Type type)
{
    this->type = type;
    this->size = this->count * getSize(type);
}

void Symbol::setAddress(int address)
{
    this->address = address;
}

void Symbol::setLine(int line)
{
    this->declare_line = line;
}

bool Symbol::updateSize(int count)
{
    bool status = false;

    // If not initialized
    if (this->size == 1 || count <= this->count)
    {
        // Update
        this->count = count;
        this->size = count * getSize(this->type);

        // Return true
        status = true;
    }

    return status;
}

// GETTERS

Category Symbol::getCategory()
{
    return this->data->getCategory();
}

Nature Symbol::getNature()
{
    return this->nature;
}

Type Symbol::getType()
{
    return this->type;
}

const char *Symbol::getName()
{
    return this->data->getName();
}

int Symbol::getLine()
{
    return this->declare_line;
}

std::vector<Symbol *> Symbol::getParams()
{
    return this->params;
}

int Symbol::getOccupiedSize()
{
    return this->size;
}

int Symbol::getAddress()
{
    return this->address;
}

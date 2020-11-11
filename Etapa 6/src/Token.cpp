#include "Token.h"

Token::Token(int line_, Category category_, Value value_, int field_)
{
    this->line = line_;
    this->category = category_;
    this->field = field_;
    this->value = value_;

    // Give name to token according to it's type
    this->name = generateName();
}

Token::Token(Token &token)
{
    this->line = token.line;
    this->category = token.category;
    this->field = token.field;

    // If string, id, special character or composite operator
    if (this->field == 0)
    {
        // Copy string memory
        this->value.name = strdup(token.value.name);
    }
    // Otherwise
    else
    {
        // Copy entire value
        this->value = token.value;
    }

    // Give name to token according to it's type
    this->name = generateName();
}

char *Token::generateName()
{
    char *name = NULL;

    switch (this->field)
    {
    case 0: // String/id/special character
        name = strdup(this->value.name);
        break;
    case 1: // Int
        name = strdup(std::to_string(this->value.integer).c_str());
        break;
    case 2: // Float
        name = strdup(std::to_string(this->value.floating).c_str());
        break;
    case 3: // Char
        name = strdup(std::string(1, this->value.character).c_str());
        break;
    case 4: // Bool
        name = strdup(this->value.boolean ? "true" : "false");
        break;
    default:
        name = NULL;
        break;
    }

    return name;
}

Token::~Token()
{
    // Free string field if necessary
    if (this->field == 0 && this->category != CAT_RESERVED)
        free(this->value.name);

    // Free name
    free(this->name);
}

Category Token::getCategory()
{
    return this->category;
}

Value Token::getValue()
{
    return this->value;
}

int Token::getLine()
{
    return this->line;
}

char *Token::getName()
{
    return this->name;
}

int Token::getField()
{
    return this->field;
}

#include "Exceptions.h"

DeclaredException::DeclaredException(Symbol *inserted_, Symbol *existed_)
{
    this->existing = existed_;
    this->inserted = inserted_;
}

std::string DeclaredException::what()
{
    std::string error;

    error = "On line " + std::to_string(get_line_number()) + ": Redeclaration of symbol " + inserted->getName() + ": " +
            "\n " + inserted->getDeclaration() +
            "\nPreviously declared on line " + std::to_string(existing->getLine()) + ": " +
            "\n " + existing->getDeclaration();

    return error;
}

UndeclaredException::UndeclaredException(std::string key_)
{
    this->key = key_;
}

std::string UndeclaredException::what()
{
    std::string error;

    error = "On line " + std::to_string(get_line_number()) + ": Symbol " + this->key + " undeclared (First use in this function).";

    return error;
}

WrongTypeException::WrongTypeException(Node *usage_)
{
    this->usage = usage_;
}

std::string WrongTypeException::what()
{
    std::string error;

    error = "On line: " + std::to_string(get_line_number()) + ": Assignment from incompatible types: \n" + usage->reconstruct() + "\nLval has type " + getTypeName(usage->getChild(0)->getType()) + "but rval has type " + getTypeName(usage->getChild(1)->getType());

    return error;
}

WrongUsageException::WrongUsageException(Symbol *symbol_, Node *usage_)
{
    this->symbol = symbol_;
    this->usage = usage_;

    switch (symbol_->getNature())
    {
    case NAT_FUNCTION:
        this->code = ERR_FUNCTION;
        break;
    case NAT_IDENTIFIER:
        this->code = ERR_VARIABLE;
        break;
    case NAT_VECTOR:
        this->code = ERR_VECTOR;
        break;
    default:
        this->code = -1;
        break;
    }
}

int WrongUsageException::getCode()
{
    return this->code;
}

std::string WrongUsageException::what()
{
    std::string error;

    switch (this->code)
    {
    case ERR_VARIABLE:
        error = "On line " + std::to_string(get_line_number()) + ": Incorrect use of variable: \n" + this->usage->reconstruct() + "\nDeclared on line " + std::to_string(this->symbol->getLine()) +
                "\n" + this->symbol->toString();
        break;
    case ERR_VECTOR:
        error = "On line " + std::to_string(get_line_number()) + ": Incorrect use of vector: \n" + this->usage->reconstruct() + "\nDeclared on line " + std::to_string(this->symbol->getLine()) +
                "\n" + this->symbol->toString();
        break;
    case ERR_FUNCTION:
        error = "On line " + std::to_string(get_line_number()) + ": Incorrect use of function: \n" + this->usage->reconstruct() + "\nDeclared on line " + std::to_string(this->symbol->getLine()) +
                "\n" + this->symbol->toString();
        break;
    default:
        error = "On line " + std::to_string(get_line_number()) + ": UNKOWN VARIABLE";
        break;
    }

    return error;
}

InvalidConversionException::InvalidConversionException(Node *usage_, Type type1_, Type type2_)
{
    this->usage = usage_;
    this->type1 = type1_;
    this->type2 = type2_;

    if (type1 == TYPE_STRING || type2 == TYPE_STRING)
    {
        this->code = ERR_STRING_TO_X;
    }
    else
    {
        this->code = ERR_CHAR_TO_X;
    }
}

int InvalidConversionException::getCode()
{
    return this->code;
}

std::string InvalidConversionException::what()
{
    std::string error;

    error = "On line " + std::to_string(get_line_number()) + ": Invalid conversion from " + getTypeName(type1) + " to " + getTypeName(type2) + ":\n " + usage->reconstruct();

    return error;
}

InvalidStringSizeException::InvalidStringSizeException(Node *usage_, Symbol *symbol_, int size_)
{
    this->usage = usage_;
    this->symbol = symbol_;
    this->size = size_;
}

std::string InvalidStringSizeException::what()
{
    std::string error;

    error = "On line " + std::to_string(get_line_number()) + ": Invalid size in string assignment: " +
            "\n " + usage->reconstruct() +
            "\n\"" + symbol->getName() + "\" has declared size " + std::to_string(symbol->getOccupiedSize()) + " but rval has size " + std::to_string(size);

    return error;
}

WrongParameterException::WrongParameterException(Node *usage_, Type expected_, Type given_, Symbol *func_)
{
    this->usage = usage_;
    this->expected = expected_;
    this->given = given;
    this->func = func_;

    // Based on statement kind
    switch (usage->getKind())
    {
    case ST_IO:
        if (!strcmp(usage->getValue()->getName(), "input"))
            this->code = ERR_WRONG_PAR_INPUT;
        else if (!strcmp(usage->getValue()->getName(), "output"))
            this->code = ERR_WRONG_PAR_OUTPUT;
        break;
    case ST_RETURN:
        this->code = ERR_WRONG_PAR_RETURN;
        break;
    default:
        this->code = -1;
        break;
    }
}

int WrongParameterException::getCode()
{
    return this->code;
}

std::string WrongParameterException::what()
{
    std::string error;

    switch (this->code)
    {
    case ERR_WRONG_PAR_INPUT:
        error = "On line " + std::to_string(get_line_number()) + ": Wrong parameter for INPUT statement: " +
                "\n " + usage->reconstruct() +
                "\nExpected INT or FLOAT, but argument has type " + getTypeName(usage->getChild(0)->getType());
        break;
    case ERR_WRONG_PAR_OUTPUT:
        error = "On line " + std::to_string(get_line_number()) + ": Wrong parameter for OUTPUT statement: " +
                "\n " + usage->reconstruct() +
                "\nExpected INT or FLOAT, but argument has type " + getTypeName(usage->getChild(0)->getType());
        break;
    case ERR_WRONG_PAR_RETURN:

        error = "On line " + std::to_string(get_line_number()) + ": Wrong type in return statement " +
                "\n " + usage->reconstruct() +
                "\nFunction has return type " + getTypeName(func->getType()) + " but expression has type " + getTypeName(usage->getChild(0)->getType()) +
                "\nFunction was declared on line " + std::to_string(func->getLine()) + ": " +
                "\n " + func->getDeclaration();
        break;
    default:
        error = "On line " + std::to_string(get_line_number()) + ": UNKOWN ERROR TYPE";
        break;
    }

    return error;
}

WrongShiftAmountException::WrongShiftAmountException(Node *usage_)
{
    this->usage = usage_;
    this->amount = usage->getChild(1)->getValue()->getValue().integer;
}

std::string WrongShiftAmountException::what()
{
    std::string error;

    error = "On line " + std::to_string(usage->getLine()) + ": Expected shift amount to be less than " + std::to_string(SHIFT_MAX) + " but got " + std::to_string(amount) + ": " +
            "\n " + usage->reconstruct();

    return error;
}

WrongArgumentsException::WrongArgumentsException(Node *usage_, Symbol *func_, int code_, int index_)
{
    this->usage = usage_;
    this->func = func_;
    this->code = code_;
    this->index = index_;
}

int WrongArgumentsException::getCode()
{
    return this->code;
}

std::string WrongArgumentsException::what()
{
    std::string error;

    switch (this->code)
    {
    case ERR_WRONG_TYPE_ARGS:
        error = "On line " + std::to_string(get_line_number()) + ": Incorrect type for argument " + std::to_string(index) + " in call: " +
                "\n " + usage->reconstruct() +
                "\nExpected " + getTypeName(func->getParams().at(index)->getType()) + " but got " + getTypeName(usage->getChild(index)->getType()) +
                "\nFunction was declared on line " + std::to_string(func->getLine()) + ": " +
                "\n " + func->getDeclaration();
        break;
    case ERR_MISSING_ARGS:
        error = "On line " + std::to_string(get_line_number()) + ": Too few arguments in function call: " +
                "\n" + usage->reconstruct() +
                "\nFunction was declared on line " + std::to_string(func->getLine()) + ": " +
                "\n " + func->getDeclaration();
        break;
    case ERR_EXCESS_ARGS:
        error = "On line " + std::to_string(get_line_number()) + ": Excess arumgnet in function call: " +
                "\n " + usage->reconstruct() +
                "\nFunction was declared on line " + std::to_string(func->getLine()) + ": " +
                "\n " + func->getDeclaration();
        break;
    default:
        error = "On line " + std::to_string(get_line_number()) + ": UNKOWN ERROR";
        break;
    }

    return error;
}

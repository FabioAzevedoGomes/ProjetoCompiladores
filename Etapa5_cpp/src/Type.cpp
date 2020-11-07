#include "Type.h"

int getSize(Type type)
{
    switch (type)
    {
    case TYPE_INT:
        return 4;
    case TYPE_FLOAT:
        return 8;
    case TYPE_BOOL:
        return 4;
    case TYPE_CHAR:
        return 1;
    case TYPE_STRING:
        return 1;
    default:
        return 0;
    }
}

bool isCompatible(Type type1, Type type2)
{
    switch (type1)
    {
    case TYPE_STRING:
        if (type2 != TYPE_STRING && type2 != TYPE_ANY)
            return false;
        break;
    case TYPE_CHAR:
        if (type2 != TYPE_CHAR && type2 != TYPE_ANY)
            return false;
        break;
    default:
        break;
    }

    switch (type2)
    {
    case TYPE_STRING:
        if (type1 != TYPE_STRING && type1 != TYPE_ANY)
            return false;
        break;
    case TYPE_CHAR:
        if (type1 != TYPE_CHAR && type1 != TYPE_ANY)
            return false;
        break;
    default:
        break;
    }

    return true;
}

const char *getTypeName(Type type)
{
    switch (type)
    {
    case TYPE_INT:
        return "int";
    case TYPE_FLOAT:
        return "float";
    case TYPE_BOOL:
        return "bool";
    case TYPE_CHAR:
        return "char";
    case TYPE_STRING:
        return "string";
    case TYPE_ANY:
        return "ANY";
    default:
        return "INVALID_TYPE";
    }
}

Type inferType(Type type1, Type type2)
{
    Type inferred_type = TYPE_ANY;

    // Check if types are compatible before inferring
    if (isCompatible(type1, type2))
    {
        // If types are the same, return type1
        if (type1 == type2)
        {
            inferred_type = type1;
        }

        // If any type is FLOAT, return FLOAT
        if (type1 == TYPE_FLOAT || type2 == TYPE_FLOAT)
            inferred_type = TYPE_FLOAT;

        // If any type is BOOL or ANY, return the other type
        if (type1 == TYPE_BOOL || type1 == TYPE_ANY)
            inferred_type = type2;
        if (type2 == TYPE_BOOL || type2 == TYPE_ANY)
            inferred_type = type1;
    }

    return inferred_type;
}

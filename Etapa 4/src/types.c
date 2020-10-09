#include "types.h"

int type_size(LanguageType type)
{
    switch (type)
    {
    case TYPE_INT:
        return 4;
    case TYPE_FLOAT:
        return 8;
    case TYPE_BOOL:
        return 1;
    case TYPE_CHAR:
        return 1;
    case TYPE_STRING:
        return 1;
    default:
        return 0;
    }
}

int compatible_types(LanguageType type1, LanguageType type2)
{

    switch (type1)
    {
    case TYPE_STRING:
        if (type2 != TYPE_STRING)
            return 0;
        break;
    case TYPE_CHAR:
        if (type2 != TYPE_CHAR)
            return 0;
        break;
    default:
        break;
    }

    switch (type2)
    {
    case TYPE_STRING:
        if (type1 != TYPE_STRING)
            return 0;
        break;
    case TYPE_CHAR:
        if (type1 != TYPE_CHAR)
            return 0;
        break;
    default:
        break;
    }

    return 1;
}

LanguageType infer_type(LanguageType type1, LanguageType type2)
{
    LanguageType inferred_type = TYPE_INT; // TODO Set to int for debugging tree construction, change here to -1 when done

    if (compatible_types(type1, type2))
    {
        // If types are the same, return type1
        if (type1 == type2)
        {
            inferred_type = type1;
        }

        // If any type is FLOAT, return FLOAT
        if (type1 == TYPE_FLOAT || type2 == TYPE_FLOAT)
            inferred_type = TYPE_FLOAT;

        // If any type is BOOL, return the other type
        if (type1 == TYPE_BOOL)
            inferred_type = type2;
        if (type2 == TYPE_BOOL)
            inferred_type = type1;
    }

    return inferred_type;
}

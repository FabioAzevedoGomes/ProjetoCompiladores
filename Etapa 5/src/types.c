#include "types.h"

void free_lexical_value(lexical_value_t *lexval, LanguageType type)
{
    if (lexval != NULL)
    {
        switch (lexval->category)
        {
        case CAT_COMPOSITE_OPERATOR: // Composite operators such as &&
        case CAT_IDENTIFIER:         // Identifiers
        case CAT_SPECIAL_CHARACTER:  // Special characters such as '.' and ';'

            // Free lexval name
            free(lexval->value.name);
            break;
        case CAT_LITERAL: // Literal values

            // If it is a string, free strduped value
            if (type == TYPE_STRING)
                free(lexval->value.string);

            break;
        default:
            break;
        }

        // And finally free lexval
        free(lexval);
    }
}

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
        if (type2 != TYPE_STRING && type2 != TYPE_ANY)
            return 0;
        break;
    case TYPE_CHAR:
        if (type2 != TYPE_CHAR && type2 != TYPE_ANY)
            return 0;
        break;
    default:
        break;
    }

    switch (type2)
    {
    case TYPE_STRING:
        if (type1 != TYPE_STRING && type1 != TYPE_ANY)
            return 0;
        break;
    case TYPE_CHAR:
        if (type1 != TYPE_CHAR && type1 != TYPE_ANY)
            return 0;
        break;
    default:
        break;
    }

    return 1;
}

LanguageType infer_type(LanguageType type1, LanguageType type2)
{
    LanguageType inferred_type = -1;

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

        // If any type is BOOL or ANY, return the other type
        if (type1 == TYPE_BOOL || type1 == TYPE_ANY)
            inferred_type = type2;
        if (type2 == TYPE_BOOL || type2 == TYPE_ANY)
            inferred_type = type1;
    }

    return inferred_type;
}

char *type_name(LanguageType type)
{
    switch (type)
    {
    case TYPE_INT:
        return "INT";
    case TYPE_FLOAT:
        return "FLOAT";
    case TYPE_BOOL:
        return "BOOL";
    case TYPE_CHAR:
        return "CHAR";
    case TYPE_STRING:
        return "STRING";
    case TYPE_TBA:
        return "TO BE ASSIGNED";
    case TYPE_NA:
        return "NOT APPLICABLE";
    default:
        return "INVALID_TYPE";
    }
}
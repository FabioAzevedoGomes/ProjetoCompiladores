#include <stdio.h>
#include <stdlib.h>

#include "tree.h"

// FREE MEMORY

/**
 * @brief Recursively removes all of the children of given node
 * @param root Pointer to the node of the AST that will be removed
*/
void free_node(void *root)
{
    node_t *node = (node_t *)root; // Reference to the node

    // Auxiliary pointers
    node_t *aux_1 = NULL;
    node_t *aux_2 = NULL;
    node_t *swap = NULL;

    // If node exists
    if (root != NULL)
    {
        // If this node has any children
        if (node->children != NULL)
        {
            // Get reference to first and second child
            aux_1 = node->children;
            aux_2 = aux_1->brothers;

            while (aux_2 != NULL || aux_1 != NULL)
            {
                // Remove the child i
                if (aux_1 != NULL)
                    free_node(aux_1);

                // Update second pointer to child i+1
                if (aux_2 != NULL)
                    aux_1 = aux_2->brothers;
                else
                    aux_1 = NULL;

                // Swap pointers
                swap = aux_1;
                aux_1 = aux_2;
                aux_2 = swap;
            }
        }

        // If this node has subsequent commands, also free them
        if (node->next_cmd != NULL)
            free_node(node->next_cmd);

        // Free memory used for lexical value of this node
        if (node->lexval != NULL)
            free_lexical_value(node->lexval, node->type);

        // Free node
        free(root);
    }
}

extern void libera(void *arvore)
{
    // Remove all nodes from the AST recursively
    free_node(arvore);
}

// EXPORT AST

/**
 * @brief Prints information about a node of the AST
 * Format:
 * 0x000000 [label='X'];
 * @param root Pointer to the node being printed
 */
void print_node(node_t *root)
{
    if (root != NULL)
    {
        // Print the common part for every node
        printf("%p, [label=\"", root);

        switch (root->lexval->category)
        {
        case CAT_SPECIAL_CHARACTER:
        case CAT_COMPOSITE_OPERATOR:
        case CAT_RESERVED:
        case CAT_IDENTIFIER:

            // If the identifier is a function call, print "call" before the name
            if (root->st_kind == CMD_FUNCTION_CALL)
                printf("call ");

            // Print identifier name
            printf("%s", root->lexval->value.name);
            break;
        case CAT_LITERAL:

            switch (root->type)
            {
            case TYPE_INT:
                printf("%d", root->lexval->value.integer);
                break;
            case TYPE_FLOAT:
                printf("%f", root->lexval->value.floating);
                break;
            case TYPE_CHAR:
                printf("%c", root->lexval->value.character);
                break;
            case TYPE_BOOL:
                root->lexval->value.boolean ? printf("true") : printf("false");
                break;
            case TYPE_STRING:
                printf("%s", root->lexval->value.string);
                break;
            default: // Error
                printf("%p", root->lexval);
                break;
            }
            break;
        }

        // Print the common end to every node
        printf("\"];\n");
    }
}

/**
 * @brief Prints informatino about the current node's connections
 * Format:
 * 0x000000, 0x000000
 * @param root Pointer to the node who's connections will be printed
 */
void print_connections(node_t *root)
{
    node_t *aux; // Auxiliary pointer for traversing the tree

    if (root != NULL)
    {
        // Get reference to the first child node
        aux = root->children;

        // Traverse children
        while (aux != NULL)
        {
            // Prints the connection
            printf("%p, %p\n", root, aux);

            // Moves to next child
            aux = aux->brothers;
        }

        // Prints the connection to next command, if it exists
        if (root->next_cmd != NULL)
            printf("%p, %p\n", root, root->next_cmd);
    }
}

/**
 * @brief Recursively exports all of the AST nodes to stdout.
 * @param root Pointer to the root of the AST
 */
void export_ast_nodes(node_t *root)
{
    if (root != NULL)
    {
        // Print information about this node
        print_node(root);

        // Print information about this node's children, brother and subsequent command
        export_ast_nodes(root->children);
        export_ast_nodes(root->brothers);
        export_ast_nodes(root->next_cmd);
    }
}

/**
 * @brief Recursively exports all of the AST connections between nodes
 * @param root Pointer to the root of the AST
 */
void export_ast_connections(node_t *root)
{
    if (root != NULL)
    {
        // Print connections for current node
        print_connections(root);

        // Print connections for this node's children, brother and subsequent command
        export_ast_connections(root->children);
        export_ast_connections(root->brothers);
        export_ast_connections(root->next_cmd);
    }
}

extern void exporta(void *arvore)
{
    // Only export ASTs that exist
    if (arvore != NULL)
    {
        // Print information about AST relations
        export_ast_connections((node_t *)arvore);

        // Print information about AST nodes
        export_ast_nodes((node_t *)arvore);
    }
}

// GENERAL NODE METHODS

node_t *create_lexical_node(lexical_value_t *lexval, LanguageType type, Statement st_kind, int declare)
{
    error_t *status = NULL; // Current operation status
    char *name = NULL;
    int size = 1;

    // Allocate memory for node
    node_t *new_node = (node_t *)malloc(sizeof(node_t));

    // Initialize lexical value, statement kind and language type
    new_node->lexval = lexval;
    new_node->st_kind = st_kind;

    // If this is an ID that is not being declared
    if (!declare && st_kind == CMD_OPERAND && type == TYPE_TBA)
    {
        // Search symbol table for it's type
        status = find_id(lexval->value.name, 1);

        // If symbol was not found
        if (status->error_type != 0)
        {
            // Print error info and exit
            print_error(status);
        }

        // Update type
        new_node->type = ((symbol_t *)(status->data1))->type;

        // Free container
        free(status);
    }
    else
    {
        // If not, just uupdate type
        new_node->type = type;
    }

    // If this is a literal that is being created
    if (lexval != NULL && lexval->category == CAT_LITERAL)
    {
        name = get_symbol_name(lexval, type);

        // Search in symbol table
        status = find_id(name, 0);

        if (status->error_type != 0)
        {
            // Symbol does not exist, declare it
            // If this is a string, update size
            if (type == TYPE_STRING)
            {
                size = strlen(lexval->value.string);
            }
            insert_symbol((symbol_table_t *)(stack->top->data), create_symbol(lexval, type, KIND_NONE, size, 0, NULL));
        }
        else
        {
            // Symbol exists, update it's data
            ((symbol_t *)(status->data1))->declaration_line = lexval->line;
        }

        free(status);
        free(name);
    }

    // On creation, node is completly disconnected from the tree
    new_node->brothers = NULL;
    new_node->children = NULL;
    new_node->next_cmd = NULL;

    // Return created node
    return new_node;
}

node_t *create_command_chain(node_t *first, node_t *second)
{
    if (first != NULL)
    {
        // Insert node as next command
        first->next_cmd = second;

        return first;
    }
    else
    {
        // If there was not first command, second becomes first
        return second;
    }
}

node_t *insert_child(node_t *father, node_t *child)
{
    // Auxiliary pointer for traversing children
    node_t *aux = NULL;

    if (father != NULL)
    {
        // Get reference to first child
        if ((aux = father->children) != NULL)
        {
            // Traverse children until reaching the last one
            while (aux->brothers != NULL)
            {
                aux = aux->brothers;
            }

            // Insert new child
            aux->brothers = child;
        }
        else
        {
            // If there was no first child
            father->children = child;
        }

        // Return father node
        return father;
    }
    else
    {
        // If father was null, return child
        return child;
    }
}

/**
 * @brief Creates a node from raw data, instead of a lexical value
 * @param category The category for this node's token
 * @param value    The value for this node's token 
 * @param type     The type of this node, TYPE_TBA if not known
 * @param st_kind  The kind of statement for this node
 * @param line     Line where the token occurred
 * @returns Pointer to the created node
 */
node_t *create_artificial_node(TokenCategory category, TokenValue value, LanguageType type, Statement st_kind, int line)
{
    // Allocate memory for lexical value
    lexical_value_t *lexval = (lexical_value_t *)malloc(sizeof(lexical_value_t));
    lexval->value = value;
    lexval->line = line;
    lexval->category = category;

    // Creates a new node using this lexval
    return create_lexical_node(lexval, type, st_kind, 1);
}

// STATEMENT NODE CREATION METHODS

node_t *create_if(node_t *condition, node_t *true_statement, node_t *false_statement)
{
    error_t *status = NULL; // Current operation status

    // Create new node for IF statement
    node_t *if_node = NULL;

    TokenValue value;
    value.name = "if";

    // Create node
    if_node = create_artificial_node(CAT_RESERVED, value, TYPE_NA, CMD_IF, condition->lexval->line);

    // Add condition statement as first child
    if_node = insert_child(if_node, condition);

    // Add true statement as second child
    if_node = insert_child(if_node, true_statement);

    // Add false statement as third child (If it is null nothing is added)
    if_node = insert_child(if_node, false_statement);

    // If condition is a variable
    if (condition->lexval->category == CAT_IDENTIFIER)
    {
        // Check for correct usage
        check_correct_id_usage(if_node, condition, condition->st_kind == CMD_FUNCTION_CALL ? KIND_FUNCTION : KIND_IDENTIFIER);
    }

    // Check if condition type is compatible
    if (!compatible_types(TYPE_BOOL, condition->type))
    {
        // If type is not compatible with bool, it is string or char
        if (condition->type == TYPE_CHAR)
        {
            // Create error
            status = create_error(ERR_CHAR_TO_X);
        }
        else if (condition->type == TYPE_STRING)
        {
            // Create error
            status = create_error(ERR_STRING_TO_X);
        }
        else
        {
            // Unknown type being used in expression
            status = create_error(-1);
        }

        // Fill error data
        status->data1 = (void *)if_node; // Incorrect usage that generated the error

        // Output error and exit
        print_error(status);
    }

    return if_node;
}

node_t *create_for(node_t *init_atrib, node_t *condition, node_t *loop_atrib, node_t *statement)
{
    // Create new node for FOR statement
    node_t *for_node = NULL;
    error_t *status = NULL;

    TokenValue value;
    value.name = "for";

    // Create node
    for_node = create_artificial_node(CAT_RESERVED, value, TYPE_NA, CMD_FOR, init_atrib->lexval->line);

    // Add initial attribution as first child
    for_node = insert_child(for_node, init_atrib);

    // Add stop condition as second child
    for_node = insert_child(for_node, condition);

    // Add loop attribution as third child
    for_node = insert_child(for_node, loop_atrib);

    // Add loop statement as fourth child
    for_node = insert_child(for_node, statement);

    // If condition is a variable
    if (condition->lexval->category == CAT_IDENTIFIER)
    {
        // Check for correct usage
        check_correct_id_usage(for_node, condition, condition->st_kind == CMD_FUNCTION_CALL ? KIND_FUNCTION : KIND_IDENTIFIER);
    }

    // Check if types are correct
    if (!compatible_types(TYPE_BOOL, condition->type))
    {
        // If type is not compatible with bool, it is string or char
        if (condition->type == TYPE_CHAR)
        {
            // Create error
            status = create_error(ERR_CHAR_TO_X);
        }
        else if (condition->type == TYPE_STRING)
        {
            // Create error
            status = create_error(ERR_STRING_TO_X);
        }
        else
        {
            // Unknown type being used in expression
            status = create_error(-1);
        }

        // Fill error data
        status->data1 = (void *)for_node; // Incorrect usage that generated the error

        // Output error and exit
        print_error(status);
    }

    return for_node;
}

node_t *create_while(node_t *condition, node_t *statement)
{
    // Create node for WHILE statement
    node_t *while_node = NULL;
    error_t *status = NULL;

    TokenValue value;
    value.name = "while";

    // Create node
    while_node = create_artificial_node(CAT_RESERVED, value, TYPE_NA, CMD_WHILE, condition->lexval->line);

    // Insert condition as first child
    while_node = insert_child(while_node, condition);

    // Insert statement as second child
    while_node = insert_child(while_node, statement);

    // If condition is a variable
    if (condition->lexval->category == CAT_IDENTIFIER)
    {
        // Check for correct usage
        check_correct_id_usage(while_node, condition, condition->st_kind == CMD_FUNCTION_CALL ? KIND_FUNCTION : KIND_IDENTIFIER);
    }

    // Check if condition type is compatible
    if (!compatible_types(TYPE_BOOL, condition->type))
    {
        // If type is not compatible with bool, it is string or char
        if (condition->type == TYPE_CHAR)
        {
            // Create error
            status = create_error(ERR_CHAR_TO_X);
        }
        else if (condition->type == TYPE_STRING)
        {
            // Create error
            status = create_error(ERR_STRING_TO_X);
        }
        else
        {
            // Unknown type being used in expression
            status = create_error(-1);
        }

        // Fill error data
        status->data1 = (void *)while_node; // Incorrect usage that generated the error

        // Output error and exit
        print_error(status);
    }

    return while_node;
}

node_t *create_vector_access(node_t *identifier, node_t *index)
{
    // Create new node for vector access
    node_t *vector_access_node = NULL;
    error_t *status = NULL;

    TokenValue value;
    value.name = "[]";

    // Create node
    vector_access_node = create_artificial_node(CAT_RESERVED, value, identifier->type, CMD_VECTOR_ACCESS, identifier->lexval->line);

    // Add identifier as first child
    vector_access_node = insert_child(vector_access_node, identifier);

    // Add indexing expression as second node
    vector_access_node = insert_child(vector_access_node, index);

    // Update vector node type to the identifier type
    vector_access_node->type = identifier->type;

    // Check if identifier is a vector
    check_correct_id_usage(vector_access_node, identifier, KIND_VECTOR);

    // Free container
    free(status);
    status = NULL;

    // Check if indexing expression type is correct
    if (!compatible_types(TYPE_INT, index->type))
    {
        // If type is not compatible with bool, it is string or char
        if (index->type == TYPE_CHAR)
        {
            // Create error
            status = create_error(ERR_CHAR_TO_X);
        }
        else if (index->type == TYPE_STRING)
        {
            // Create error
            status = create_error(ERR_STRING_TO_X);
        }
        else
        {
            // Unknown type being used in expression
            status = create_error(-1);
        }

        // Fill error data
        status->data1 = (void *)vector_access_node; // Incorrect usage that generated the error

        // Output error and exit
        print_error(status);
    }

    return vector_access_node;
}

node_t *create_input(node_t *identifier)
{
    // Create a node for "input"
    node_t *input_node = NULL;
    error_t *status = NULL;

    TokenValue value;
    value.name = "input";

    // Create node
    input_node = create_artificial_node(CAT_RESERVED, value, TYPE_NA, CMD_IO, identifier->lexval->line);

    // Insert identifier as first child
    input_node = insert_child(input_node, identifier);

    // Check for correct usage
    check_correct_id_usage(input_node, identifier, identifier->st_kind == CMD_FUNCTION_CALL ? KIND_FUNCTION : KIND_IDENTIFIER);

    // If identifier has the wrong type
    if (!compatible_types(identifier->type, TYPE_INT))
    {
        // Create error
        status = create_error(ERR_WRONG_PAR_INPUT);

        // Fill error data
        status->data1 = (void *)input_node;

        // Print error and exit
        print_error(status);
    }

    // Add the identifier as only child
    return input_node;
}

node_t *create_output(node_t *operand)
{
    // Create a node for "output"
    node_t *output_node = NULL;
    error_t *status = NULL;

    TokenValue value;
    value.name = "output";

    // Create node
    output_node = create_artificial_node(CAT_RESERVED, value, TYPE_NA, CMD_IO, operand->lexval->line);

    // Insert operand as only child
    output_node = insert_child(output_node, operand);

    // If operand is a variable
    if (operand->lexval->category == CAT_IDENTIFIER)
    {
        // Check for correct usage
        check_correct_id_usage(output_node, operand, operand->st_kind == CMD_FUNCTION_CALL ? KIND_FUNCTION : KIND_IDENTIFIER);
    }

    // If identifier has the wrong type
    if (!compatible_types(operand->type, TYPE_INT))
    {
        // Create error
        status = create_error(ERR_WRONG_PAR_OUTPUT);

        // Fill error data
        status->data1 = (void *)output_node; // Usage that caused the error

        // Print error and exit
        print_error(status);
    }

    // Add the operand as the only child
    return output_node;
}

node_t *create_continue(int line)
{
    // Wrap "continue" in a TokenValue union
    TokenValue value;
    value.name = "continue";

    // Create node
    return create_artificial_node(CAT_RESERVED, value, TYPE_NA, CMD_BREAK_CONTINUE, line);
}

node_t *create_break(int line)
{
    // Wrap "continue" in a TokenValue union
    TokenValue value;
    value.name = "break";

    // Create node
    return create_artificial_node(CAT_RESERVED, value, TYPE_NA, CMD_BREAK_CONTINUE, line);
}

node_t *create_return(node_t *retval)
{
    // Creates a node for "return" command
    node_t *return_node = NULL;
    error_t *status = NULL;

    TokenValue value;
    value.name = "return";

    // Create node
    return_node = create_artificial_node(CAT_RESERVED, value, TYPE_NA, CMD_RETURN, retval->lexval->line);

    // Insert return value as only child
    return_node = insert_child(return_node, retval);

    // Check if retval is an identifier
    if (retval->lexval->category == CAT_IDENTIFIER)
    {
        // Check for correct usage
        check_correct_id_usage(return_node, retval, retval->st_kind == CMD_FUNCTION_CALL ? KIND_FUNCTION : KIND_IDENTIFIER);
    }

    // Check if inside a function (redundant, should always be inside the function)
    if (current_function != NULL)
    {
        // Check if return type is compatible with function type
        if (!compatible_types(((symbol_t *)(current_function->data))->type, retval->type))
        {
            // Create error
            status = create_error(ERR_WRONG_PAR_RETURN);

            // Fill error data
            status->data1 = current_function->data; // Declaration of function
            status->data2 = (void *)retval;         // Usage that caused the error

            // Print error and exit
            print_error(status);
        }
    }

    // Return return node
    return return_node;
}

node_t *create_shift(node_t *identifier, node_t *amount, node_t *operator)
{
    // Creates a node for the shift operation
    node_t *shift_node = NULL;
    error_t *status = NULL;

    // Adds the identifier as first child
    shift_node = insert_child(operator, identifier);

    // Adds the shift amount as the second child
    shift_node = insert_child(shift_node, amount);

    // Update shift type to operand type
    shift_node->type = identifier->type;

    // Check if id is an identifier
    if (identifier->lexval->category == CAT_IDENTIFIER)
    {
        // Check for correct usage
        check_correct_id_usage(shift_node, identifier, identifier->st_kind == CMD_FUNCTION_CALL ? KIND_FUNCTION : KIND_IDENTIFIER);
    }

    // Check if shift operand has the correct size
    if (amount->lexval->value.integer > 16)
    {
        // Create error
        status = create_error(ERR_WRONG_PAR_SHIFT);

        // Fill error data
        status->data1 = (void *)(&(amount->lexval->value.integer)); // Incorrect usage that generated the error

        // Print error and exit
        print_error(status);
    }

    return shift_node;
}

node_t *create_attrib(node_t *lval, node_t *rval, node_t *operator)
{
    // Create a node for the attribution operation
    node_t *attrib_node = NULL;
    error_t *status = NULL;

    symbol_t *lval_symbol = NULL;
    int string_size = 0;

    // Add lval as first child
    attrib_node = insert_child(operator, lval);

    // Add rval as second child
    attrib_node = insert_child(attrib_node, rval);

    // Update attribution type (The identifier type prevails)
    attrib_node->type = lval->type;

    // Check if rval is an identifier
    if (rval->lexval->category == CAT_IDENTIFIER)
    {
        // Check correct usage
        check_correct_id_usage(attrib_node, rval, rval->st_kind == CMD_FUNCTION_CALL ? KIND_FUNCTION : KIND_IDENTIFIER);
    }

    // Check if lval is an identifier
    if (lval->lexval->category == CAT_IDENTIFIER)
    {
        // Check correct usage
        check_correct_id_usage(attrib_node, lval, lval->st_kind == CMD_FUNCTION_CALL ? KIND_FUNCTION : KIND_IDENTIFIER);
    }

    // If types are not compatible
    if (!compatible_types(lval->type, rval->type))
    {
        // Create error
        status = create_error(ERR_WRONG_TYPE);

        // Fill error data
        status->data1 = (void *)attrib_node;

        // output error and exit
        print_error(status);
    }

    // Check if this is an attribution to a string
    // Note: If no error was generated, then lval is also of type string
    if (lval->type == TYPE_STRING)
    {
        // Get reference to rval symbol
        status = find_id(lval->lexval->value.name, 1);

        // No error was generated before so we can skip existance check
        lval_symbol = ((symbol_t *)(status->data1));

        // Free container
        free(status);
        status = NULL;

        // Get size of resulting rval string
        string_size = get_resulting_string_size(rval);

        // Check if string has been initialized
        if (lval_symbol->count == 0)
        {
            // If not, then only define it's count and size
            lval_symbol->count = string_size;
            lval_symbol->size = lval_symbol->count * type_size(lval_symbol->type);
        }
        else
        {
            // If it has, check if assignement respects initial size
            if (lval_symbol->count < string_size)
            {
                // If not, generate error
                status = create_error(ERR_STRING_SIZE);

                // Fill data
                status->data1 = (void *)lval_symbol;
                status->data2 = (void *)attrib_node;
                status->data3 = (void *)&string_size;

                // Output error and exit
                print_error(status);
            }
        }
    }

    return attrib_node;
}

node_t *create_init(node_t *identifier, node_t *rval, node_t *operator)
{
    // Create node for variable initialization
    node_t *init_node = NULL;

    // Add identifier as first child
    init_node = insert_child(operator, identifier);

    // Add rval as second child
    init_node = insert_child(init_node, rval);

    return init_node;
}

node_t *create_unop(node_t *operation, node_t *operand)
{
    LanguageType new_type; // New type for the unary operation

    // Create a new node for the unary operator
    node_t *unop_node = NULL;
    error_t *status = NULL;

    // Check for new expression type
    new_type = infer_type(operation->type, operand->type);

    // Insert operand as the only child
    unop_node = insert_child(operation, operand);

    // Check if operand is an ID
    if (operand->lexval->category == CAT_IDENTIFIER)
    {
        // Check for correct usage
        check_correct_id_usage(unop_node, operand, operand->st_kind == CMD_FUNCTION_CALL ? KIND_FUNCTION : KIND_IDENTIFIER);
    }

    // Update unop node type
    unop_node->type = new_type;

    // If types are not compatible
    if (new_type == -1)
    {
        // It is string or char
        if (operand->type == TYPE_CHAR)
        {
            // Create error
            status = create_error(ERR_CHAR_TO_X);
        }
        else if (operand->type == TYPE_STRING)
        {
            // Create error
            status = create_error(ERR_STRING_TO_X);
        }
        else
        {
            // Unknown type being used in expression
            status = create_error(-1);
        }

        // Fill error data
        status->data1 = (void *)unop_node; // Incorrect usage that generated the error

        // Output error and exit
        print_error(status);
    }

    return unop_node;
}

node_t *create_binop(node_t *operation, node_t *lval, node_t *rval)
{
    LanguageType new_type; // New type for the binary operation
    error_t *status = NULL;

    // Create a node for the binary operation
    node_t *binop_node = NULL;

    // Complete the children
    binop_node = insert_child(operation, lval);
    binop_node = insert_child(binop_node, rval);

    // If lval is an ID
    if (lval->lexval->category == CAT_IDENTIFIER)
    {
        // Check for correct usage
        check_correct_id_usage(binop_node, lval, lval->st_kind == CMD_FUNCTION_CALL ? KIND_FUNCTION : KIND_IDENTIFIER);
    }

    // if rval is an IF
    if (rval->lexval->category == CAT_IDENTIFIER)
    {
        // Check for correct usage
        check_correct_id_usage(binop_node, rval, rval->st_kind == CMD_FUNCTION_CALL ? KIND_FUNCTION : KIND_IDENTIFIER);
    }

    // If the operation is a ternary operator
    if (operation->st_kind == CMD_TERNOP)
    {
        // If condition is not bool compatible
        if (!compatible_types(TYPE_BOOL, lval->type))
        {
            // It is string or char
            if (lval->type == TYPE_CHAR)
            {
                // Create error
                status = create_error(ERR_CHAR_TO_X);
            }
            else if (lval->type == TYPE_STRING)
            {
                // Create error
                status = create_error(ERR_STRING_TO_X);
            }
            else
            {
                // Unknown type being used in expression
                status = create_error(-1);
            }

            // Fill error data
            status->data1 = (void *)binop_node; // Incorrect usage that generated the error

            // Output error and exit
            print_error(status);
        }

        // If false statement is not compatible with true statement
        if ((new_type = infer_type(operation->children->type, rval->type)) == -1)
        {
            status = create_error(ERR_WRONG_TYPE);
            status->data1 = (void *)binop_node;

            // Output error and exit
            print_error(status);
        }

        // Update node type
        binop_node->type = new_type;
    }
    else
    {
        // Check if operand types are compatible with operation
        new_type = infer_type(lval->type, operation->type);

        // If they are not compatible
        if (new_type == -1)
        {
            // It is string or char
            if (lval->type == TYPE_CHAR)
            {
                // Create error
                status = create_error(ERR_CHAR_TO_X);
            }
            else if (lval->type == TYPE_STRING)
            {
                // Create error
                status = create_error(ERR_STRING_TO_X);
            }
            else
            {
                // Unknown type being used in expression
                status = create_error(-1);
            }

            // Fill error data
            status->data1 = (void *)binop_node; // Incorrect usage that generated the error

            // Output error and exit
            print_error(status);
        }

        // Check if operation type is compatible
        new_type = infer_type(rval->type, operation->type);

        if (new_type == -1)
        {
            // It is string or char
            if (rval->type == TYPE_CHAR)
            {
                // Create error
                status = create_error(ERR_CHAR_TO_X);
            }
            else if (rval->type == TYPE_STRING)
            {
                // Create error
                status = create_error(ERR_STRING_TO_X);
            }
            else
            {
                // Unknown type being used in expression
                status = create_error(-1);
            }

            // Fill error data
            status->data1 = (void *)binop_node; // Incorrect usage that generated the error

            // Output error and exit
            print_error(status);
        }

        // If any operand is string, both have to be string
        if ((lval->type == TYPE_STRING || rval->type == TYPE_STRING) && lval->type != rval->type)
        {
            // Create error
            status = create_error(ERR_STRING_TO_X);

            // Fill error data
            status->data1 = binop_node;

            // Output error and exit
            print_error(status);
        }

        // Update binop node type
        binop_node->type = infer_type(lval->type, rval->type);
    }

    return binop_node;
}

node_t *create_ternop(node_t *true_statement)
{
    // Create a node for the ternary operator
    node_t *ternop_node = NULL;

    TokenValue value;
    value.name = ":?";

    // Partially create the ternary operator node
    ternop_node = create_artificial_node(CAT_RESERVED, value, true_statement->type, CMD_TERNOP, true_statement->lexval->line);

    // Insert the true statement as first child
    ternop_node = insert_child(ternop_node, true_statement);

    // If the statement is an identifier
    if (true_statement->lexval->category == CAT_IDENTIFIER)
    {
        // Check for correct usage
        check_correct_id_usage(true_statement, true_statement, true_statement->st_kind == CMD_FUNCTION_CALL ? KIND_FUNCTION : KIND_IDENTIFIER);
    }

    return ternop_node;
}

node_t *create_function_declaration(node_t *identifier, node_t *body, LanguageType type)
{
    // Create node for function declaration
    node_t *function_declaration_node = NULL;

    // Insert function body as first child
    function_declaration_node = insert_child(identifier, body);

    // Update function type
    function_declaration_node->type = type;

    return function_declaration_node;
}

node_t *create_function_call(node_t *identifier, node_t *args)
{
    // Create node for function call
    node_t *function_call_node = NULL;
    error_t *status = NULL;
    symbol_t *symbol = NULL;

    st_entry_t *aux_param = NULL; // Auxiliary pointer for traversing function parameters
    node_t *aux_args = NULL;      // Auxiliary pointer for traversing function arguments

    // Insert argument list as the first child
    function_call_node = insert_child(identifier, args);

    // Mark node as function call
    function_call_node->st_kind = CMD_FUNCTION_CALL;

    // Get reference to identifier from symbol table
    status = find_id(identifier->lexval->value.name, 1);

    // If identifier was not found (redundant)
    if (status->error_type != 0)
    {
        // Output error and exit
        print_error(status);
    }

    // Keep reference to symbol
    symbol = ((symbol_t *)(status->data1));

    // Update function type
    function_call_node->type = symbol->type;

    // Free container
    free(status);
    status = NULL;

    // Check for correct usage
    check_correct_id_usage(function_call_node, function_call_node, KIND_FUNCTION);

    // Get reference to first function parameter, arugment and child
    aux_param = symbol->args;
    aux_args = args;

    // Number of arguments being passed to the function
    int arg_count = 0;

    // Check passed arguments
    while (aux_args != NULL)
    {
        // Based on node kind
        switch (aux_args->st_kind)
        {
        case CMD_OPERAND: // ID or literal, no children
            if (status == NULL && aux_param != NULL && aux_args->lexval->category == CAT_IDENTIFIER)
            {
                // Check correct usage
                check_correct_id_usage(function_call_node, aux_args, aux_args->st_kind == CMD_FUNCTION_CALL ? KIND_FUNCTION : KIND_IDENTIFIER);
            }

            // Check if corresponding parameter has the same type
            if (aux_param != NULL && !compatible_types(aux_args->type, ((symbol_t *)(aux_param->data))->type))
            {
                // Create error
                status = create_error(ERR_WRONG_TYPE_ARGS);

                // Fill error data
                status->data1 = (void *)symbol;
                status->data2 = (void *)function_call_node;
                status->data3 = &arg_count;

                // Only exit after processing all arguments
            }
            // Move to next argument
            aux_args = aux_args->children;

            break;
        case CMD_VECTOR_ACCESS: // Vector acess, 2 children

            if (status == NULL && aux_param != NULL)
            {
                // Check if corresponding parameter has the same type
                if (!compatible_types(aux_args->type, ((symbol_t *)(aux_param->data))->type))
                {
                    // Create error
                    status = create_error(ERR_WRONG_TYPE_ARGS);

                    // Fill error data
                    status->data1 = (void *)symbol;
                    status->data2 = (void *)function_call_node;
                    status->data3 = &arg_count;

                    // Only exit after processing all arguments
                }
            }

            // Move to next argument (3rd child)
            aux_args = aux_args->children->brothers->brothers;

            break;
        case CMD_FUNCTION_CALL: // Function call, 1 child
            if (status == NULL && aux_param != NULL)
            {
                // Check if corresponding parameter has the same type
                if (!compatible_types(aux_args->type, ((symbol_t *)(aux_param->data))->type))
                {
                    // Create error
                    status = create_error(ERR_WRONG_TYPE_ARGS);

                    // Fill error data
                    status->data1 = (void *)symbol;
                    status->data2 = (void *)function_call_node;
                    status->data3 = &arg_count;

                    // Only exit after processing all arguments
                }
            }

            // Move to next argument (3rd child)
            aux_args = aux_args->children->brothers;

            break;
        case CMD_UNOP: // Unary operatrion, 1 child
            if (status == NULL && aux_param != NULL)
            {
                // Check if corresponding parameter has the same type
                if (!compatible_types(aux_args->type, ((symbol_t *)(aux_param->data))->type))
                {
                    // Create error
                    status = create_error(ERR_WRONG_TYPE_ARGS);

                    // Fill error data
                    status->data1 = (void *)symbol;
                    status->data2 = (void *)function_call_node;
                    status->data3 = &arg_count;

                    // Only exit after processing all arguments
                }
            }

            // Move to next argument (2nd child)
            aux_args = aux_args->children->brothers;

            break;
        case CMD_BINOP: // Binary operation, 2 children

            if (status == NULL && aux_param != NULL)
            {

                // Check if corresponding parameter has the same type
                if (!compatible_types(aux_args->type, ((symbol_t *)(aux_param->data))->type))
                {
                    // Create error
                    status = create_error(ERR_WRONG_TYPE_ARGS);

                    // Fill error data
                    status->data1 = (void *)symbol;
                    status->data2 = (void *)function_call_node;
                    status->data3 = &arg_count;

                    // Only exit after processing all arguments
                }
            }

            // Move to next argument (3rd child)
            aux_args = aux_args->children->brothers->brothers;

            break;
        case CMD_TERNOP: // Ternary operation, 3 children

            if (status == NULL && aux_param != NULL)
            {
                // Check if corresponding parameter has the same type
                if (!compatible_types(aux_args->type, ((symbol_t *)(aux_param->data))->type))
                {
                    // Create error
                    status = create_error(ERR_WRONG_TYPE_ARGS);

                    // Fill error data
                    status->data1 = (void *)symbol;
                    status->data2 = (void *)function_call_node;
                    status->data3 = &arg_count;

                    // Only exit after processing all arguments
                }
            }
            // Move to next argument (4th child)
            aux_args = aux_args->children->brothers->brothers->brothers;

            break;
        default: // Error, should never happen
            // Create unknown error
            status = create_error(-1);

            // Output error and exit
            print_error(status);
            break;
        }

        // If there are still parameters in the function delcaration
        if (aux_param != NULL)
        {
            // Move to next parameter
            aux_param = aux_param->next;
        }

        // Increase argument count
        arg_count++;
    }

    // If different amounts of passed and expected arguments
    if (arg_count != symbol->argument_count)
    {
        // Create error
        status = create_error(arg_count < symbol->argument_count ? ERR_MISSING_ARGS : ERR_EXCESS_ARGS);

        // Fill error data
        status->data1 = (void *)symbol;             // Function declaration
        status->data2 = (void *)function_call_node; // Incorrect usage that generated the error

        // Output error and exit
        print_error(status);
    }

    // If an error occurred during argument check
    if (status != NULL)
    {
        // Output error and exit
        print_error(status);
    }

    return function_call_node;
}

// TYPE AND ID CHECKING METHODS

void check_correct_id_usage(node_t *usage, node_t *id, SymbolKind mode)
{
    error_t *status = NULL;
    symbol_t *symbol = NULL;

    // Get identifier
    status = find_id(id->lexval->value.name, 1);

    // Check if it exists
    if (status->error_type != 0)
    {
        print_error(status);
    }

    // Get reference to symbol
    symbol = ((symbol_t *)(status->data1));

    // Free container
    free(status);
    status = NULL;

    // Based on symbol kind
    switch (symbol->kind)
    {
    case KIND_IDENTIFIER: // If symbol is an identifier
        if (mode != KIND_IDENTIFIER)
            status = create_error(ERR_VARIABLE);
        break;
    case KIND_VECTOR:
        if (mode != KIND_VECTOR)
            status = create_error(ERR_VECTOR);
        break;
    case KIND_FUNCTION:
        if (mode != KIND_FUNCTION)
            status = create_error(ERR_FUNCTION);
        break;
    default:
        break;
    }

    // Check if was used correctly
    if (status != NULL)
    {
        // Fill error data
        status->data1 = (void *)symbol;
        status->data2 = (void *)usage;

        // Output error and exit
        print_error(status);
    }
}

int get_resulting_string_size(node_t *node)
{
    int length = 0;
    error_t *status = NULL;

    // The only situations where string size is known is on binop and identifiers/literals
    switch (node->st_kind)
    {
    case CMD_OPERAND:
        if (node->lexval->category == CAT_IDENTIFIER)
        {
            // If id, get symbol from table
            status = find_id(node->lexval->value.name, 1);

            // Get size (We can skip checking because no error was generated for this node)
            length = ((symbol_t *)(status->data1))->count;
        }
        else
        {
            // If string literal, get size
            length = strlen(node->lexval->value.string);
        }

        break;
    case CMD_BINOP: // If binop, it is a concatenation of two strings

        // Get both the string's length and sum them
        length = get_resulting_string_size(node->children) + get_resulting_string_size(node->children->brothers);

        break;
    default:
        break;
    }

    return length;
}

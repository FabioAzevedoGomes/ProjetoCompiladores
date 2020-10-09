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

void free_lexical_value(lexical_value_t *lexval, LanguageType type)
{
    if (lexval != NULL)
    {
        switch (lexval->category)
        {
        case CAT_COMPOSITE_OPERATOR: // Composite operators such as &&
        case CAT_IDENTIFIER:         // Identeifiers
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

node_t *create_lexical_node(lexical_value_t *lexval, LanguageType type, Statement st_kind)
{
    // Allocate memory for node
    node_t *new_node = (node_t *)malloc(sizeof(node_t));

    // Initialize lexical value, statement kind and language type
    new_node->lexval = lexval;
    new_node->st_kind = st_kind;
    new_node->type = type;

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
    return create_lexical_node(lexval, type, st_kind);
}

// STATEMENT NODE CREATION METHODS

node_t *create_if(node_t *condition, node_t *true_statement, node_t *false_statement)
{
    // Create new node for IF statement
    node_t *if_node = NULL;

    TokenValue value;
    value.name = "if";

    // Check if condition type is compatible
    if (compatible_types(TYPE_BOOL, condition->type))
    {
        // Create node
        if_node = create_artificial_node(CAT_RESERVED, value, TYPE_NA, CMD_IF, condition->lexval->line);

        // Add true statement as first child
        if_node = insert_child(if_node, true_statement);

        // Add false statement as third child (If it is null nothing is added)
        if_node = insert_child(if_node, false_statement);
    }

    return if_node;
}

node_t *create_for(node_t *init_atrib, node_t *condition, node_t *loop_atrib, node_t *statement)
{
    // Create new node for FOR statement
    node_t *for_node = NULL;

    TokenValue value;
    value.name = "for";

    // Check if types are correct
    if (compatible_types(TYPE_BOOL, condition->type))
    {
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
    }

    return for_node;
}

node_t *create_while(node_t *condition, node_t *statement)
{
    // Create node for WHILE statement
    node_t *while_node = NULL;

    TokenValue value;
    value.name = "while";

    // Check if condition type is compatible
    if (compatible_types(TYPE_BOOL, condition->type))
    {
        // Create node
        while_node = create_artificial_node(CAT_RESERVED, value, TYPE_NA, CMD_WHILE, condition->lexval->line);

        // Insert condition as first child
        while_node = insert_child(while_node, condition);

        // Insert statement as second child
        while_node = insert_child(while_node, statement);
    }

    return while_node;
}

node_t *create_vector_access(node_t *identifier, node_t *index)
{
    // Create new node for vector access
    node_t *vector_access_node = NULL;

    TokenValue value;
    value.name = "[]";

    // Check if indexing expression type is correct
    if (index->type == TYPE_INT)
    {
        // Create node
        vector_access_node = create_artificial_node(CAT_SPECIAL_CHARACTER, value, identifier->type, CMD_VECTOR_ACCESS, identifier->lexval->line);

        // Add identifier as first child
        vector_access_node = insert_child(vector_access_node, identifier);

        // Add indexing expression as second node
        vector_access_node = insert_child(vector_access_node, index);
    }

    return vector_access_node;
}

node_t *create_input(node_t *identifier)
{
    // Create a node for "input"
    node_t *input_node = NULL;

    TokenValue value;
    value.name = "input";

    input_node = create_artificial_node(CAT_RESERVED, value, TYPE_NA, CMD_IO, identifier->lexval->line);

    // Add the identifier as only child
    return insert_child(input_node, identifier);
}

node_t *create_output(node_t *operand)
{
    // Create a node for "output"
    node_t *output_node = NULL;

    TokenValue value;
    value.name = "output";

    output_node = create_artificial_node(CAT_RESERVED, value, TYPE_NA, CMD_IO, operand->lexval->line);

    // Add the operand as the only child
    return insert_child(output_node, operand);
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

    TokenValue value;
    value.name = "return";

    return_node = create_artificial_node(CAT_RESERVED, value, TYPE_NA, CMD_RETURN, retval->lexval->line);

    // Insert return expression as the only child
    return insert_child(return_node, retval);
}

node_t *create_shift(node_t *identifier, node_t *amount, node_t *operator)
{
    // Creates a node for the shift operation
    node_t *shift_node = NULL;

    // Adds the identifier as first child
    shift_node = insert_child(operator, identifier);

    // Adds the shift amount as the second child
    shift_node = insert_child(shift_node, amount);

    // Update shift type to operand type
    shift_node->type = identifier->type;

    return shift_node;
}

node_t *create_attrib(node_t *lval, node_t *rval, node_t *operator)
{
    LanguageType new_type; // New type for the attribution

    // Create a node for the attribution operation
    node_t *attrib_node = NULL;

    // Check for type compatibility
    new_type = infer_type(lval->type, rval->type);

    // If compatible
    if (new_type != -1)
    {
        // Add lval as first child
        attrib_node = insert_child(operator, lval);

        // Add rval as second child
        attrib_node = insert_child(attrib_node, rval);
    }

    return attrib_node;
}

node_t *create_init(node_t *identifier, node_t *rval, node_t *operator)
{
    LanguageType new_type; // New type for the initialization

    // Create node for variable initialization
    node_t *init_node = NULL;

    // Check if types are compatible
    new_type = infer_type(identifier->type, rval->type);

    // If they are
    if (new_type != -1)
    {
        // Add identifier as first child
        init_node = insert_child(operator, identifier);

        // Add rval as second child
        init_node = insert_child(init_node, rval);
    }

    return init_node;
}

node_t *create_unop(node_t *operation, node_t *operand)
{
    LanguageType new_type; // New type for the unary operation

    // Create a new node for the unary operator
    node_t *unop_node = NULL;

    // Check for type compatibility
    new_type = infer_type(operation->type, operand->type);

    // If types are compatible
    if (new_type != -1)
    {
        // Insert operand as the only child
        unop_node = insert_child(operation, operand);
    }

    return unop_node;
}

node_t *create_binop(node_t *operation, node_t *lval, node_t *rval)
{

    LanguageType new_type; // New type for the binary operation

    // Create a node for the binary operation
    node_t *binop_node = NULL;

    // Check if operand types are compatible
    new_type = infer_type(lval->type, rval->type);

    // Check if operation type is compatible
    new_type = infer_type(operation->type, new_type);

    // If they are
    if (new_type != -1)
    {
        // Insert lval as first child
        binop_node = insert_child(operation, lval);

        // Insert rval as second child
        binop_node = insert_child(binop_node, rval);
    }

    return binop_node;
}

node_t *create_ternop(node_t *condition, node_t *true_statement, node_t *false_statement)
{
    LanguageType new_type; // New type for the ternary operation

    // Create a node for the ternary operator
    node_t *ternop_node = NULL;

    TokenValue value;
    value.name = ":?";

    // Check if expression types are compatible
    new_type = infer_type(true_statement->type, false_statement->type);

    // If condition also has the right type
    if (new_type != -1 && compatible_types(TYPE_BOOL, condition->type))
    {
        // Create node
        ternop_node = create_artificial_node(CAT_SPECIAL_CHARACTER, value, new_type, CMD_TERNOP, condition->lexval->line);

        // Add condition as first child
        ternop_node = insert_child(ternop_node, condition);

        // Add true statement as second child
        ternop_node = insert_child(ternop_node, true_statement);

        // Add false statemente as second child
        ternop_node = insert_child(ternop_node, false_statement);
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
    LanguageType new_type = TYPE_TBA; // New type for the function call

    // Create node for function call
    node_t *function_call_node = NULL;

    // TODO Check for argument, identifier and return typings in symbol table, update node type with fucntion return type

    // Update function return type
    identifier->type = new_type;

    // Insert argument list as the first child
    function_call_node = insert_child(identifier, args);

    return function_call_node;
}

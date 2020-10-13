#include <stdio.h>
#include <stdlib.h>

#include "error_report.h"

error_t *create_error(int type)
{
    error_t *error = (error_t *)malloc(sizeof(error_t));

    // Initialize error type and data count
    error->error_type = type;
    error->data1 = NULL;
    error->data2 = NULL;
    error->data3 = NULL;

    // Return pointer to created structure
    return error;
}

/**
 * @brief Reconstructs the code for a given node, for better error reporting 
 * @param node Node being reconstructed
 */
char *reconstruct_node(node_t *node)
{
    // TODO
    return " * NOT IMPLEMENTED * ";
}

void print_error(error_t *error)
{
    // Print an error line start:
    printf("[ERROR] On line %d: ", get_line_number());

    if (error != NULL)
    {
        switch (error->error_type)
        {
        case ERR_UNDECLARED:
            printf("Identifier \"%s\" undeclared (first use in this function)\n", (char *)error->data1);
            break;
        case ERR_DECLARED:
            printf("Redeclaration of \"%s\"\n[ERROR] Previous declaration of \"%s\" was on line %d\n", ((symbol_t *)(error->data1))->key, ((symbol_t *)(error->data1))->key, ((symbol_t *)(error->data2))->declaration_line);
            break;
        case ERR_VARIABLE:
            printf("Incorrect use of variable \"%s\":\n\n %s \n\n[ERROR] Declared on line %d\n", ((symbol_t *)(error->data1))->key, reconstruct_node((node_t *)(error->data2)), ((symbol_t *)(error->data1))->declaration_line);
            break;
        case ERR_VECTOR:
            printf("Incorrect use of vector \"%s\":\n\n %s \n\n[ERROR] Declared on line %d\n", ((symbol_t *)(error->data1))->key, reconstruct_node((node_t *)(error->data2)), ((symbol_t *)(error->data1))->declaration_line);
            break;
        case ERR_FUNCTION:
            printf("Incorrect use of function \"%s\":\n\n %s \n\n[ERROR] Declared on line %d\n", ((symbol_t *)(error->data1))->key, reconstruct_node((node_t *)(error->data2)), ((symbol_t *)(error->data1))->declaration_line);
            break;
        case ERR_WRONG_TYPE:
            printf("Wrong type\n");
            break;
        case ERR_STRING_TO_X:
            printf("No implicit conversion for STRING\n");
            // TODO Descobrir o tipo para o qual a string esta sendo convertida
            // TODO Verificar se o erro foi gerado por uma variavel ou por uma constante, se for variavel printar a declaracao e / ou atribuicao
            break;
        case ERR_CHAR_TO_X:
            printf("No implicit conversion for CHAR \n");
            // TODO Descobrir o tipo para o qual o char esta sendo convertido
            // TODO verificar se o erro foi gerado por uma variavel ou por uma constante, se for variavel princtar a declaracao e / ou atribuicao
            break;
        case ERR_STRING_SIZE:
            printf("Incorrect size for STRING\n");
            // TODO Descobrir o tamanho associado com aquela string, e o tamanho do valor sendo usado para a atribuicao
            // TODO Talvez mostrar a declaracao e / ou atribuicao inicial da string
            break;
        case ERR_MISSING_ARGS:
            printf("Too few arguments in function call:\n\n %s \n\n[ERROR] Declared on line %d\n", reconstruct_node((node_t *)(error->data2)), ((symbol_t *)(error->data1))->declaration_line);
            // TODO Maybe reconstruct declaration of function for clarity
            break;
        case ERR_EXCESS_ARGS:
            printf("Too many arguments in function call:\n\n %s \n\n[ERROR] Declared on line %d\n", reconstruct_node((node_t *)(error->data2)), ((symbol_t *)(error->data1))->declaration_line);
            // TODO Maybe reconstruct declaration of function for claritys
            break;
        case ERR_WRONG_TYPE_ARGS:
            printf("Incorrect argument type in argument %d of function call:\n\n %s \n\n[ERROR] Exepected <EXPECTED-TYPE>>, but found <EXPECTED-ARG>\n", *((int *)(error->data3)), reconstruct_node((node_t *)(error->data2)));
            break;
        case ERR_WRONG_PAR_INPUT:
            printf("WRONG_PAR_INPUT");
            break;
        case ERR_WRONG_PAR_OUTPUT:
            printf("WRONG_PAR_OUTPUT");
            break;
        case ERR_WRONG_PAR_RETURN:
            printf("WRONG_PAR_RETURN");
            break;
        case ERR_WRONG_PAR_SHIFT:
            printf("WRONG_PAR_SHIFT");
            break;
        default:
            printf("Invalid error code received from parser: %d\n", error->error_type);
            break;
        }

        exit(error->error_type);
    }
    else
    {
        // NULL error, should never happen
        printf("Received NULL error descriptor.\n This should never happen, aborting.");
        exit(-1);
    }
}

/**
 * This file contains definitions for errors that should be returned from other methods
 * This also defines some methods for creating these errors descriptors 
 */
#ifndef ERROR_H
#define ERROR_H

#include "errors.h"
#include "types.h"

/**
 * @brief Defines the format for error descriptions to be returned  
 */
typedef struct error_return_format
{
    int error_type; // Type of error that ocurred (See errors.h) or 0 when no errors ocurred

    // Pointers for storing data related to the error
    void *data1;
    void *data2;
    void *data3;

} error_t;

#include "symbol_table.h" // Symbol data type

/**
 * @brief Defined in scanner.l, get current line number
 * @returns Current line number
 */
int get_line_number();

/**
 * @brief Allocates memory for an error descriptor and initializes it
 * @param type The type for the error being reported
 * @returns Pointer to the error structure 
 */
error_t *create_error(int type);

/**
 * @brief Prints information about a semantic error to stdout and exists with error status code
 * @param error The error description
 */
void print_error(error_t *error);

char *reconstruct_node(node_t *node);

#endif
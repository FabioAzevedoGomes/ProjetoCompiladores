/**
 * This file contains the data structures needed for creating the ILOC compliant
 * command list 
 */

#ifndef ILOC_H
#define ILOC_H

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "types.h"

/**
 * @brief Creates a new command entry with the given data, following the convention
 *                  opcode arg1, arg2 => arg3 
 *                            or 
 *                  opcode arg1 => arg2, arg3 (for stores and cbr)
 * @param opcode Operation code for the command
 * @param arg1   First argument of the command
 * @param arg2   Second argument of the command
 * @param arg3   Third argumet of the command
 */
iloc_t *create_iloc_command(ILOCop opcode, void *arg1, void *arg2, void *arg3);

/**
 * @brief Adds a new ILOC command in the given index
 * @param index Position where the new command is to be inserted
 * @param first First command in the list
 * @param command Command being inserted
 * @returns Pointer to the new list
 */
iloc_list_t *add_command(int index, iloc_list_t *first, iloc_t *command);

/**
 * @brief Generates a name for a new label, according to
 * ILOC standard convention:
 * L[0-9]+
 */
char *get_label();

/**
 * @brief Generates a name for a new register, according to
 * ILOC standard conventions:
 * r[0-9]+
 */
char *get_register();

/**
 * @brief Outputs the code in ILOC syntax to stdout
 * @param first First command in the list
 */
void output_code(iloc_list_t *first);

#endif
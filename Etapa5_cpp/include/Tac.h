/**
 * This file contains the TAC class, used for representing the Tree-Acess-Code generated
 * for the language, according to ILOC standard:
 * 
 *  opcode arg1, arg2 => arg3       | (Most operations)
 *              or
 *  opcode arg1       => arg2, arg3 | (Stores and conditional branch)
 *              or
 *  opcode arg1       => arg2       | (Simple load/store)
 *              or
 *  opcode arg1                     | (Jumps)
 * 
 * Author: Fábio de Azevedo Gomes
 */
#ifndef TAC_H
#define TAC_H

#include "Type.h"

#include <string>
#include <sstream>

class Tac
{
private:
    static uint register_count; // Number of registers used
    static uint label_count;    // Number of temporary labels used

    ILOCop opcode; // TAC Operation code

    std::string arg1; // First argument
    std::string arg2; // Second argument
    std::string arg3; // Third argument

    std::string label; // Label for this instruction, if there is one

    // Double linked list
    Tac *next; // Next instruction
    Tac *prev; // Previous instruction

public:
    /**
     * @brief Creates a new register name for usage with temporary variables
     * @returns A new register name, previously unused
     */
    static std::string newRegister();

    /**
     * @brief Creates a new label name for usage with jumps
     * @returns A new label name, previously unused 
     */
    static std::string newLabel();

    // CONSTRUCTOR AND DESTRUCTOR

    /**
     * @brief Class constructor
     * @param opcode_ Operation code for this instruction
     * @param arg1_   First argument
     * @param arg2_   Second argument
     * @param arg3_   Third argument
     */
    Tac(ILOCop opcode_, std::string arg1_, std::string arg2_, std::string arg3_);

    /**
     * @brief Class destructor 
     */
    ~Tac();

    // GETTERS

    /**
     * @brief Returns a string containing the entire instruction chain
     * that follows this one, inclusive, one instruction per line:
     * 
     *  opcode arg1, arg2 => arg3;
     *  opcode arg1, arg2 => arg3;
     *  ...
     * @returns String containig the ILOC code
     */
    std::string getCodeString();

    /**
     * @brief Retruns this instruction's label, if there is one  
     */
    std::string getLabel();

    // SETTERS

    /**
     * @brief Sets this instruction's label to the given one 
     */
    void setLabel(std::string label);

    // INSTRUCTION LIST LOGIC

    /**
     * @brief Adds another instruction before this one 
     * @param instruction New instruction being added
     */
    void addBefore(Tac *instruction);

    /**
     * @brief Adds another instruction after this one 
     * @param instruction New instruction being added
     */
    void addAfter(Tac *instruction);

    // EXPORT

    /**
     * @brief Converts this instruction to an ILOC instruction
     * @returns a string containing this operation in ILOC standard format 
     */
    std::string toString();
};

#endif

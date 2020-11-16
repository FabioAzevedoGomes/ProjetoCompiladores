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
#include "ASM.h"

#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <algorithm>

// Forward declare ASM
class ASM;

class Tac
{
private:
    static uint register_count; // Number of registers used
    static uint label_count;    // Number of temporary labels used
    static uint hole_count;     // Number of label holes used

    // List containing a reference to every name that was generated (Registers, labels and patched holes)
    static std::list<std::string *> used_names;

    ILOCop opcode; // TAC Operation code

    std::string *arg1; // First argument
    std::string *arg2; // Second argument
    std::string *arg3; // Third argument

    std::string *label; // Label for this instruction, if there is one

    // Double linked list
    Tac *next; // Next instruction
    Tac *prev; // Previous instruction

    // Live variables in this instruciton
    std::vector<std::string *> live_in;
    std::vector<std::string *> live_out;

public:
    /**
     * @brief Creates a new register name for usage with temporary variables
     * @returns A new register name, previously unused
     */
    static std::string *newRegister();

    /**
     * @brief Creates a new label name for usage with jumps
     * @returns A new label name, previously unused 
     */
    static std::string *newLabel();

    /**
     * @brief Creates a new "hole" for a label, to be filled by a later expression
     * @returns A new hole name, previously unused 
     */
    static std::string *newHole();

    /**
     * @brief Returns the appropriate register name given the variable global status
     * @param global If the symbol being addressed is local or global
     * @returns "rbss" or "rfp", base on given global status 
     */
    static std::string *getRegister(int global);

    /**
     * @brief Returns a new string that was saved in the global name list, used for literal values
     * @param literal The literal that should be kept track of
     * @returns String with the literal 
     */
    static std::string *getName(std::string literal);

    /**
     * @brief Patches a list of holes with the given label
     * @param holes List of holes to be patched
     * @param label Label to be given to those holes
     */
    static void patch(std::list<std::string *> holes, std::string *label);

    /**
     * @brief Frees all the memory used for the labels and names that were generated 
     */
    static void freeNames();

    // CONSTRUCTOR AND DESTRUCTOR

    /**
     * @brief Class constructor
     * @param opcode_ Operation code for this instruction
     * @param arg1_   First argument
     * @param arg2_   Second argument, none by default
     * @param arg3_   Third argument, none by default
     */
    Tac(ILOCop opcode_, std::string *arg1_ = NULL, std::string *arg2_ = NULL, std::string *arg3_ = NULL);

    /**
     * @brief Copy constructor
     * Copies the entire code chain present in the given tac
     */
    Tac(Tac &tac);

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
    std::string *getLabel();

    /**
     * @brief Retruns the total amount of instructions in the code chain starting
     * on this instruction 
     */
    int getCodeSize();

    /**
     * @brief Returns operation code for this instruction 
     */
    ILOCop getOpcode();

    /**
     * @brief Retruns the next instruction after this one 
     */
    Tac *getNext();

    /**
     * @brief Returns the previous instruction after this one 
     */
    Tac *getPrev();

    /**
     * @brief Returns the selected argument
     * @param index Selected arg 
     */
    std::string *getArgument(int index);

    /**
     * @brief Retruns a list with this instruction's used 
     * variables 
     */
    std::list<std::string *> getVariables();

    // SETTERS

    /**
     * @brief Sets this instruction's label to the given one 
     */
    void setLabel(std::string *label);

    // TEMPORARY VARIABLE LIVENESS

    /**
     * @brief Adds a new variable to this TAC's live-in list
     * @param temp Temporary variable's name
     */
    void addLiveIn(std::string *temp);

    /**
     * @brief Adds a new variable to this TAC's live-out list 
     * @param temp Temporary variable's name
     */
    void addLiveOut(std::string *temp);

    /**
     * @brief Removes a variable from this TAC's live-in list
     * @param temp Temporary variable's name 
     */
    void removeLiveIn(std::string *temp);

    /**
     * @brief Removes a variable from this TAC's live-out list
     * @param temp Temporary variable's name 
     */
    void removeLiveOut(std::string *temp);

    /**
     * @brief Returns the variables that are 'live' during this 
     * instruction 
     */
    std::vector<std::string *> getLive();

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

    /**
     * @brief Adds another instruction at the end of
     * the instruction chain starting with this one
     */
    void addLast(Tac *instruction);

    // EXPORT

    /**
     * @brief Converts this instruction to an ILOC instruction
     * @returns a string containing this operation in ILOC standard format 
     */
    std::string toString();
};

#endif

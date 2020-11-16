/**
 * This file models structures present in the assembly code that will
 * be generated for the input program, in accordance with the x86_64 
 * instruction set
 * 
 * Author: Fábio de Azevedo Gomes 
 */
#ifndef ASM_H
#define ASM_H

#include "Tac.h"
#include "Type.h"
#include "Manager.h"
#include "SymbolTable.h"
#include "Symbol.h"
#include "Token.h"
#include "Graph.h"

#include <unordered_map>
#include <map>
#include <sstream>
#include <algorithm>

// Forward declare Tac
class Tac;

class ASM
{
private:
    static Tac *ILOC_code; // Intermediate code to generate ASM file for

    static std::map<std::string *, Symbol *> functions; // ILOC Label to Funcion symbol map
    static Symbol *current_function;                    // Current function begin converted to ASM
    static int function_index;                          // Total written functions

    static std::map<std::string *, std::string> registers; // Temporary to register allocation map

public:
    // BROAD FUNCTIONS FOR GENERATING SEGMENTS

    /**
     * @brief Generates the ASM code for the program
     * @param first First instruction of the ILOC code
     * @returns String containing the x86_64-compliant ASM 'file'
     */
    static std::string generateASM(Tac *first);

    /**
     * @brief Generates the data segment of the ASM program
     * @returns String containing the data segment
     */
    static std::string generateDataSegment();

    /**
     * @brief Generates the code segment of the ASM program
     * @returns String conatining the code segment 
     */
    static std::string generateCodeSegment();

    /**
     * @brief Generates the ending segment of the ASM program 
     * @returns String containing the end segment
     */
    static std::string generateEndSegment();

    // SPECIFIC FUNCTIONS

    /**
     * @brief Generates code for starting a function
     * @param current First TAC of this function
     * @returns String containing the start code block
     */
    static std::string generateFunctionStart(Tac *current);

    /**
     * @brief Generates code for ending a function
     * @param current First TAC of the next function
     * @returns String containing the end code block 
     */
    static std::string generateFunctionEnd(Tac *current);

    /**
     * @brief Runs through the code performing variable 
     * liveness tests, and generates a temp - register map
     * to be used when translating this instruction's code 
     * @param first First TAC of the function
     */
    static void allocateRegisters(Tac *first);

    /**
     * @brief Generates the register interference graph 
     * for a variable liveness-anotated TAC sequence
     * @param first First instruction in the TAC sequence
     * @param temps List of temporary variables that appear in the TAC sequence
     * @returns Register interference graph for that sequence
     */
    static Graph *createRegisterInterferenceGraph(Tac *first, std::vector<std::string *> temps);

    /**
     * @brief Translates the argument to the ASM syntax
     * @param arg Argument 
     */
    static std::string translateArgument(std::string *arg);

    /**
     * @brief Translates the TAC instruction to an ASM instruction 
     * @param instruction The TAC code
     * @returns String containing the ASM instruction
     */
    static std::string translateTac(Tac *current);

    // CHECKERS

    /**
     * @brief Checks if given insstruction starts a new function
     * @param instruction The checked TAC
     * @returns True if starts a new function, false otherwise 
     */
    static bool startsNewFunction(Tac *instruction);
};

#endif

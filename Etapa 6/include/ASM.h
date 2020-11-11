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

#include <unordered_map>
#include <map>
#include <sstream>
#include <algorithm>

class ASM
{
private:
    static Tac *ILOC_code; // Intermediate code to generate ASM file for

    static std::map<std::string *, Symbol *> functions; // ILOC Label to Funcion symbol map
    static Symbol *current_function;                    // Current function begin converted to ASM
    static int function_index;                          // Total written functions

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
     * @returns String containing the start code block
     */
    static std::string generateFunctionStart();

    /**
     * @brief Generates code for ending a function
     * @returns String containing the end code block 
     */
    static std::string generateFunctionEnd();
};

#endif

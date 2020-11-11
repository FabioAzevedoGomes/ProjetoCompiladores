#include "ASM.h"

Tac *ASM::ILOC_code = NULL;
std::map<std::string *, Symbol *> ASM::functions;
Symbol *ASM::current_function = NULL;
int ASM::function_index = 0;

std::string ASM::generateASM(Tac *first)
{
    std::stringstream program;

    // Code starting point
    ASM::ILOC_code = first;

    // Generate data segment (Global vars, literals)
    program << ASM::generateDataSegment() << std::endl;

    // Generate code segment (function declaration and code)
    program << ASM::generateCodeSegment() << std::endl;

    // Generate end segment (meta-info)
    program << ASM::generateEndSegment();

    // Return generated program
    return program.str();
}

std::string ASM::generateDataSegment()
{
    int literal;
    std::stringstream data_segment;

    // Comments for legibility of generated ASM file
    data_segment << "# =======================" << std::endl
                 << "#  Start of Data Segment" << std::endl
                 << "# =======================" << std::endl;

    // Get global symbol table
    SymbolTable *global_symbol_table = Manager::getActiveSymbolTable();
    std::unordered_map<std::string, Symbol *> symbols = global_symbol_table->getHashTable();
    std::vector<Symbol *> ordered_symbols;

    // Get symbols in a vector
    for (auto i = symbols.begin(); i != symbols.end(); ++i)
        ordered_symbols.push_back((*i).second);

    // Order by memory address (Declaration order)
    std::sort(ordered_symbols.begin(), ordered_symbols.end(),
              [](Symbol *e1, Symbol *e2) { return (e1->getAddress() < e2->getAddress()); });

    // Add starting text directive
    data_segment << "  \t.text" << std::endl;

    // Iterate symbol table
    for (auto i = ordered_symbols.begin(); i != ordered_symbols.end(); ++i)
    {
        switch ((*i)->getNature())
        {
        case NAT_IDENTIFIER: // Global variable
        case NAT_VECTOR:     // Global vector
        {
            // Add symbol name and size
            data_segment << "\t.comm " << (*i)->getName() << "," << (*i)->getOccupiedSize() << std::endl;
        }
        break;
        case NAT_NONE: // Literals
        {
            // Get integer
            literal = (*i)->getValue()->getValue().integer;

            // 'Declare' byte
            data_segment << "\t.long " << literal << "\t# Literal: " << (*i)->getName() << std::endl;
        }
        break;
        case NAT_FUNCTION: // Functions
        {
            // Save the <label,function> tuple for easier access during code generation
            ASM::functions.insert(std::make_pair((*i)->getLabel(), *i));
        }
        break;
        default: // Error
            // nil
            break;
        }
    }

    // Return generated data segment
    return data_segment.str();
}

std::string ASM::generateCodeSegment()
{
    std::string *label = NULL;
    Symbol *function = NULL;
    std::stringstream code_segment;
    Tac *current = NULL;

    // Comments for legibility of generated ASM file
    code_segment << "# =======================" << std::endl
                 << "#  Start of Code Segment" << std::endl
                 << "# =======================" << std::endl;

    // Move past starting instructions untilhalt (They are covered by data segment initialization)
    for (current = ASM::ILOC_code; current->getOpcode() != ILOC_HALT; current = current->getNext())
        ;

    // Iterate instructions
    for (current = current->getNext(); current != NULL; current = current->getNext())
    {
        // Get instruction label
        if ((label = current->getLabel()) != NULL)
        {
            try
            {
                // Get function corresponding to label
                function = functions[label];

                // If ending a function's code segment
                if (current_function != NULL)
                    code_segment << generateFunctionEnd();

                // Update current function
                current_function = function;

                // Generate function initial code
                code_segment << ASM::generateFunctionStart();
            }
            catch (const std::out_of_range &e)
            {
                // Was not a function
            }

            // Reset label
            label = NULL;
        }

        // Based on instruction type
        switch (current->getOpcode())
        {
            // TODO // TODO // TODO // TODO // TODO // TODO // TODO // TODO // TODO // TODO // TODO // TODO // TODO
            // Generate code for easy instructions first, should be a 1:1 mapping
            // In some cases I might need to switch a set of instructions to a single one, or vice versa
            // TODO // TODO // TODO // TODO // TODO // TODO // TODO // TODO // TODO // TODO // TODO // TODO // TODO
        default: // Not implemented yet
            code_segment << "\tnop # Code not implemented yet for " << opname.at(current->getOpcode()) << std::endl;
            break;
        }
    }

    // When done, insert end code for last function
    code_segment << generateFunctionEnd();

    // Return generated code segment
    return code_segment.str();
}

std::string ASM::generateEndSegment()
{
    std::stringstream end_segment;

    // Comments for legibility of generated ASM file
    end_segment << "# =======================" << std::endl
                << "#           END " << std::endl
                << "# =======================" << std::endl;

    // Code generation info
    end_segment << "\t.ident \"FABOCC: 0.0.1\"" << std::endl
                << "\t.section	.note.GNU-stack,\"\",@progbits";

    // Return generated end segment
    return end_segment.str();
}

//

std::string ASM::generateFunctionStart()
{
    std::stringstream function_start;

    // Comments for legibility
    function_start << std::endl
                   << "# =======================" << std::endl
                   << "#  Start of function \"" << current_function->getName() << "\"" << std::endl
                   << "# =======================" << std::endl;

    // Add function start code
    function_start << "\t.globl " << current_function->getName() << std::endl
                   << "\t.type  " << current_function->getName() << ", @function" << std::endl;

    function_start << current_function->getName() << ":" << std::endl
                   << ".LFB" << ASM::function_index << ":" << std::endl
                   << "\t.cfi_startproc" << std::endl
                   << std::endl;

    return function_start.str();
}

std::string ASM::generateFunctionEnd()
{
    std::stringstream function_end;

    // Add function end code
    function_end << std::endl
                 << "\t.cfi_endproc" << std::endl
                 << ".LFE" << ASM::function_index << ":" << std::endl
                 << "\t.size " << current_function->getName() << ", .-" << current_function->getName() << std::endl;

    ASM::function_index++;

    // Return generated end code
    return function_end.str();
}

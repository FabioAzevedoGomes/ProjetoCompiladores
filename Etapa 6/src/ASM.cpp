#include "ASM.h"

Tac *ASM::ILOC_code = NULL;
std::map<std::string *, Symbol *> ASM::functions;
Symbol *ASM::current_function = NULL;
int ASM::function_index = 0;
std::map<std::string *, std::string> ASM::registers;

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

            // 'Declare' literal
            data_segment << "__" << (*i)->getName() << ":" << std::endl;
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

    // Move past starting instructions until halt (They are covered by data segment initialization)
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
                function = functions.at(label);

                // If ending a function's code segment
                if (current_function != NULL)
                    code_segment << ASM::generateFunctionEnd(current);

                // Update current function
                current_function = function;

                // Generate function initial code
                code_segment << ASM::generateFunctionStart(current);

                // Move past initialization instructions
                for (; current->getPrev()->getOpcode() != ILOC_ADDI; current = current->getNext())
                    ;
            }
            catch (const std::out_of_range &e)
            {
                // Was not a function
            }
        }

        // If instruction has label
        if (label != NULL)
        {
            try
            {
                functions.at(label);

                // Starts a function, omit
            }
            catch (const std::out_of_range &e)
            {
                // Does not start a function, add
                code_segment << *label << ": " << std::endl;
            }
        }

        // If function starts a return sequence
        if (current->startsReturn())
        {
            // If loading a value
            if (!current->getArgument(0)->compare("rbss"))
            {
                Symbol *symbol = Manager::getActiveSymbolTable()->getSymbolByAddress(std::stoi(*current->getArgument(1)));
                code_segment << "\tmovl __" << symbol->getName() << "(\%rip), \%eax" << std::endl;
            }
            else if (!current->getArgument(0)->compare("rfp"))
            {
                code_segment << "\tmovl -" <<  *current->getArgument(1) << "(\%rbp), \%eax" << std::endl;
            }
            else
            {
                // Add regload instruction
                code_segment << "\tmovl "
                             << ASM::translateArgument(current->getArgument(0))
                             << ", \%eax" << std::endl;
            }

            // Ignore everything until jump return
            for (; current->getOpcode() != ILOC_JUMP; current = current->getNext())
                ;
        }

        // Transate instruction to x86 ASM
        code_segment << "\t" << ASM::translateTac(current) << std::endl;

        // Reset label
        label = NULL;
    }

    // When done, insert end code for last function
    code_segment << generateFunctionEnd(current);

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
    end_segment << "\t.ident \"INF01147\"" << std::endl
                << "\t.section	.note.GNU-stack,\"\",@progbits";

    // Return generated end segment
    return end_segment.str();
}

//

std::string ASM::generateFunctionStart(Tac *current)
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
                   << "\tpushq \%rbp" << std::endl
                   << "\tmovq \%rsp, \%rbp" << std::endl;

    // Generate a register allocation map
    ASM::allocateRegisters(current);

    // Return genrated code
    return function_start.str();
}

std::string ASM::generateFunctionEnd(Tac *current)
{
    std::stringstream function_end;

    // Add function end code
    function_end << std::endl
                 << ".LFE" << ASM::function_index << ":" << std::endl
                 << "\t.size " << current_function->getName() << ", .-" << current_function->getName() << std::endl;

    ASM::function_index++;

    // Return generated end code
    return function_end.str();
}

void ASM::allocateRegisters(Tac *first)
{
    // List of temporary variables
    std::vector<std::string *> temps;

    // Last use for each variable
    std::map<std::string *, Tac *> last_usage;

    // Iterate function code adding live variables, registering their last use
    for (Tac *current = first; !ASM::startsNewFunction(current); current = current->getNext())
    {
        // Get the temporary variables used in this instruction
        std::list<std::string *> used_variables = current->getVariables();

        // For each used variable
        for (auto var = used_variables.begin(); var != used_variables.end(); ++var)
        {
            // Check if exists in the map
            try
            {
                last_usage.at(*var);

                // If so, update last usage
                last_usage[*var] = current;
            }
            catch (const std::out_of_range &e)
            {
                // If not, add it
                last_usage.insert(std::make_pair(*var, current));
                temps.push_back(*var);

                // Insert it as live out in the current instruction
                current->addLiveOut(*var);
            }
        }
    }

    // Iterate the last usage map, removing variable from live-out of last instruction
    for (auto var = last_usage.begin(); var != last_usage.end(); ++var)
    {
        // Remove from subsequent instructions
        var->second->removeLiveOut(var->first);
    }

    // Generate the register interference graph
    Graph *G = ASM::createRegisterInterferenceGraph(first, temps);

    // Try to minimize the amount of registers used
    if (G->minimizeRegisterUsage(X86_64_REGCOUNT))
    {
        // Number of used registers was minimized, get temp->register mapping
        ASM::registers = G->getColorMapping();
    }
    else
    {
        // Was not able to fit entire code in X86_64_REGCOUNT registers, must spill
        // TODO
    }

    // DEBUG
    //std::cout << G->outputInfo() << std::endl;

    // Free memory used in the register interference graph
    delete G;

    return;
}

Graph *ASM::createRegisterInterferenceGraph(Tac *first, std::vector<std::string *> temps)
{
    // Create empty graph
    Graph *g = new Graph();

    // Iterate temporary variable list creating vertexes
    for (auto i = temps.begin(); i != temps.end(); ++i)
        g->addVertex(*i);

    // Iterate code
    for (Tac *current = first; current != NULL && !ASM::startsNewFunction(current); current = current->getNext())
    {
        // Get current instruction's live_in list
        std::vector<std::string *> live_vars = current->getLive();

        // Add an edge for each pair of live variables in the current instruction
        for (auto i = live_vars.begin(); i != live_vars.end(); ++i)
            for (auto j = i + 1; j != live_vars.end(); ++j)
                g->addEdge(*i, *j);
    }

    // Return created graph
    return g;
}

std::string ASM::translateArgument(std::string *arg)
{
    std::stringstream result;

    switch (arg->at(0))
    {
    case 'r': // Register
        try
        {
            // Regular register
            std::string name = ASM::registers.at(arg);
            result << name;
        }
        catch (const std::out_of_range &e)
        {
            // Special register
            switch (arg->at(1))
            {
            case 'f': // rfp
                result << "\%rbp";
                break;
            case 's': // rsp
                result << "\%rsp";
                break;
            case 'b': // rbss
                result << "(\%rip)";
                break;
            case 'p': // rpc
                result << "\%rip";
                break;
            default: // Error
                result << "?";
                break;
            }
        }
        break;
    case 'l':           // Label
        result << *arg; // TODO not sure yet
        break;
    default: // Literal
        result << "$" << *arg;
        break;
    }

    return result.str();
}

std::string ASM::translateTac(Tac *instruction)
{
    std::stringstream code;

    // DEBUG
    code << "# TAC: " << instruction->toString() << std::endl
         << "\t";

    // Based on instruction type
    switch (instruction->getOpcode())
    {
    case ILOC_NOP: // nop
        code << "nop";
        break;
    case ILOC_ADD: // Addition
    case ILOC_ADDI:
    {
        // If arg1 = arg3
        if (!ASM::translateArgument(instruction->getArgument(0)).compare(ASM::translateArgument(instruction->getArgument(2))))
        {
            // addq r2, r1 // r1 <- r1 + r2
            code << "addq " << ASM::translateArgument(instruction->getArgument(1)) << ", " << ASM::translateArgument(instruction->getArgument(0));
        }
        // If arg2 = arg3
        else if (!ASM::translateArgument(instruction->getArgument(1)).compare(ASM::translateArgument(instruction->getArgument(2))))
        {
            // addq r1,r2 // r2 <- r1 + r2
            code << "addq " << ASM::translateArgument(instruction->getArgument(0)) << ", " << ASM::translateArgument(instruction->getArgument(1));
        }
        // Otherwise
        else
        {
            // Check if this instruction is perparing to load or store a value
            if (!instruction->getArgument(0)->compare("rbss"))
            {
                // Get symbol associated with the address
                Symbol *symbol = Manager::getActiveSymbolTable()->getSymbolByAddress(std::stoi(*instruction->getArgument(1)));

                // Based on next instruction
                if (instruction->getNext()->getOpcode() == ILOC_LOAD)
                {
                    // Add ASM to load it directly into the next instruction's register
                    code << "movl __" << symbol->getName() << "(\%rip), " << ASM::translateArgument(instruction->getArgument(2)) << "d";
                }
                else if (instruction->getNext()->getOpcode() == ILOC_STORE)
                {
                    // Add ASM to store directly to it
                    code << "movl " << ASM::translateArgument(instruction->getNext()->getArgument(2)) << "d, __" << symbol->getName() << "(\%rip)";
                }
            }
            else if (!instruction->getArgument(0)->compare("rfp"))
            {

                if (instruction->getNext()->getOpcode() == ILOC_LOAD)
                {
                    code << "movl -" << *instruction->getArgument(1) << "(\%rbp), " << ASM::translateArgument(instruction->getArgument(2)) << "d";
                }
                else if (instruction->getNext()->getOpcode() == ILOC_STORE)
                {
                    // Add ASM to store directly to it
                    code << "movl " << ASM::translateArgument(instruction->getNext()->getArgument(0)) << "d, -" << *instruction->getArgument(1) << "(\%rbp)";
                }
            }
            else
            {

                // movq r1 r3 // r3 <- r1
                code << "movq " << ASM::translateArgument(instruction->getArgument(0)) << ", " << ASM::translateArgument(instruction->getArgument(2)) << std::endl;

                // addq r2, r3 // r3 <- r2 + r3
                code << "\taddq " << ASM::translateArgument(instruction->getArgument(1)) << ", " << ASM::translateArgument(instruction->getArgument(2));
            }
        }
    }
    break;
    case ILOC_SUB: // Subtraction
    case ILOC_SUBI:
    {
        // If arg1 = arg3
        if (!ASM::translateArgument(instruction->getArgument(0)).compare(ASM::translateArgument(instruction->getArgument(2))))
        {
            // subq r2, r1
            code << "subq " << ASM::translateArgument(instruction->getArgument(1)) << ", " << ASM::translateArgument(instruction->getArgument(0));
        }
        // Otherwise
        else
        {
            // movq r1 r3
            code << "movq " << ASM::translateArgument(instruction->getArgument(0)) << ", " << ASM::translateArgument(instruction->getArgument(2)) << std::endl;

            // subq r2 r3
            code << "\tsubq " << ASM::translateArgument(instruction->getArgument(1)) << ", " << ASM::translateArgument(instruction->getArgument(2));
        }
    }
    break;
    case ILOC_RSUBI: // Subtraction (Backwards)
    {

        // movq c2 r3
        code << "movq " << ASM::translateArgument(instruction->getArgument(1)) << ", " << ASM::translateArgument(instruction->getArgument(2)) << std::endl;

        // subq r1 r3
        code << "\tsubq " << ASM::translateArgument(instruction->getArgument(0)) << ", " << ASM::translateArgument(instruction->getArgument(2));
    }
    break;
    case ILOC_MULT: // Multiplication
    case ILOC_MULTI:
    {
        code << "movq " << ASM::translateArgument(instruction->getArgument(0)) << ", \%eax " << std::endl;
        code << "\timulq " << ASM::translateArgument(instruction->getArgument(1)) << ", \%eax" << std::endl;
        code << "\tmovq \%eax, " << ASM::translateArgument(instruction->getArgument(2));
    }
    break;
    case ILOC_DIV: // Division
    case ILOC_DIVI:
    {
        code << "movq " << ASM::translateArgument(instruction->getArgument(0)) << ", \%eax" << std::endl;
        code << "\tidivq " << ASM::translateArgument(instruction->getArgument(1)) << std::endl;
        code << "\tmovq \%eax " << ASM::translateArgument(instruction->getArgument(2));
    }
    break;
    case ILOC_STORE: // Simple store
    {
        // movq r1, (r2)
        //code << "movq " << ASM::translateArgument(instruction->getArgument(0)) << ", (" << ASM::translateArgument(instruction->getArgument(1)) << ")";
    }
    break;
    case ILOC_STOREAI: // Composite store
    {
        // movq r1 val(r2)
        code << "movq " << ASM::translateArgument(instruction->getArgument(0)) << ", " << *instruction->getArgument(2) << "(" << ASM::translateArgument(instruction->getArgument(1)) << ")";
    }
    break;
    case ILOC_LOAD: // Simple memory load
    {
        // movq (r1), r2
        //code << "movq (" << ASM::translateArgument(instruction->getArgument(0)) << "), " << ASM::translateArgument(instruction->getArgument(1));
    }
    break;
    case ILOC_LOADI: // Constant load
    {
        // movq $lit, r2
        code << "movq " << ASM::translateArgument(instruction->getArgument(0)) << ", " << ASM::translateArgument(instruction->getArgument(1));
    }
    break;
    case ILOC_LOADAI: // Composite memory load
    {
        // movq lit(r1), r2
        code << "movq " << *instruction->getArgument(1) << "(" << ASM::translateArgument(instruction->getArgument(0)) << "), " << ASM::translateArgument(instruction->getArgument(2));
    }
    break;
    case ILOC_I2I: // Register copy
    {
        // movq r1, r2
        code << "movq " << ASM::translateArgument(instruction->getArgument(0)) << ", " << ASM::translateArgument(instruction->getArgument(1));
    }
    break;
    case ILOC_CMP_EQ: // Compares
    case ILOC_CMP_GE:
    case ILOC_CMP_GT:
    case ILOC_CMP_LE:
    case ILOC_CMP_LT:
    case ILOC_CMP_NE:
    {
        // cmp r1, r2
        code << "cmp " << ASM::translateArgument(instruction->getArgument(0)) << ", " << ASM::translateArgument(instruction->getArgument(1));
    }
    break;
    case ILOC_CBR: // Conditional branch
    {
        // Check last instruction (branch condition)
        switch (instruction->getPrev()->getOpcode())
        {
        case ILOC_CMP_EQ:
            code << "je  " << *instruction->getArgument(0);
            break;
        case ILOC_CMP_GE:
            code << "jg  " << *instruction->getArgument(0);
            break;
        case ILOC_CMP_GT:
            code << "jge " << *instruction->getArgument(0);
            break;
        case ILOC_CMP_LE:
            code << "jle " << *instruction->getArgument(0);
            break;
        case ILOC_CMP_LT:
            code << "jl  " << *instruction->getArgument(0);
            break;
        case ILOC_CMP_NE:
            code << "jne " << *instruction->getArgument(0);
            break;
        default: // Error
            break;
        }
    }
    break;
    case ILOC_JUMPI: // Unconditional branch to label
    {
        try
        {
            // Check if jump to function
            Symbol *function = functions.at(instruction->getArgument(0));

            // call
            code << "call " << function->getName();
        }
        catch (const std::out_of_range &e)
        {
            // Regular jump to label
            code << "jmp " << *instruction->getArgument(0);
        }
    }
    break;
    case ILOC_JUMP: // Unconditional branch to register value
    {
        code << "popq \%rbp" << std::endl
             << "\tret ";
    }
    break;
    default:
        code << "nop # No translation for " << opname.at(instruction->getOpcode());
        break;
    }

    // Return generated code string
    return code.str();
}

//

bool ASM::startsNewFunction(Tac *instruction)
{
    bool status = false;

    if (instruction == NULL)
    {
        status = true;
    }
    else if (instruction->getLabel() != NULL)
    {
        try
        {
            Symbol *function = ASM::functions.at(instruction->getLabel());

            // If starting not this function
            if (strcmp(function->getName(), current_function->getName()) != 0)
                status = true;
        }
        catch (const std::out_of_range &e)
        {
            // Does not start a function
            status = false;
        }
    }

    return status;
}

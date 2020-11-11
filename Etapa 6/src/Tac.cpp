#include "Tac.h"

// Register, label and hole counters for name generation
uint Tac::register_count = 0;
uint Tac::label_count = 0;
uint Tac::hole_count = 0;
std::list<std::string *> Tac::used_names;

std::string *Tac::newRegister()
{
    // Create new name
    std::string *new_register = new std::string("r" + std::to_string(Tac::register_count));

    // Update counter
    Tac::register_count++;

    // Add to list
    Tac::used_names.push_back(new_register);

    return new_register;
}

std::string *Tac::newLabel()
{
    std::string *new_label = new std::string("L" + std::to_string(Tac::label_count));

    // Update counter
    Tac::label_count++;

    // Add to list
    Tac::used_names.push_back(new_label);

    return new_label;
}

std::string *Tac::newHole()
{
    std::string *new_hole = new std::string("H" + std::to_string(Tac::hole_count));

    // Update counter
    Tac::hole_count++;

    // Add to list
    Tac::used_names.push_back(new_hole);

    return new_hole;
}

std::string *Tac::getRegister(int global)
{
    std::string *reg = new std::string(global ? "rbss" : "rfp");

    // Add to list
    Tac::used_names.push_back(reg);

    return reg;
}

std::string *Tac::getName(std::string literal)
{
    std::string *new_literal = new std::string(literal);

    // Add to list
    used_names.push_back(new_literal);

    return new_literal;
}

void Tac::patch(std::list<std::string *> holes, std::string *label)
{
    for (auto i = holes.begin(); i != holes.end(); ++i)
        **i = *label;

    // TODO Here we can free the hole strings as they get patched, as each hole is unique
}

void Tac::freeNames()
{
    for (auto i = Tac::used_names.begin(); i != Tac::used_names.end(); ++i)
        delete *i;
}

//

Tac::Tac(ILOCop opcode_, std::string *arg1_, std::string *arg2_, std::string *arg3_)
{
    this->opcode = opcode_;
    this->arg1 = arg1_;
    this->arg2 = arg2_;
    this->arg3 = arg3_;
    this->label = NULL;

    this->next = NULL;
    this->prev = NULL;
}

Tac::Tac(Tac &tac)
{
    Tac *chain;

    this->opcode = tac.opcode;
    this->arg1 = tac.arg1;
    this->arg2 = tac.arg2;
    this->arg3 = tac.arg3;
    this->label = tac.label;
    this->next = NULL;
    this->prev = NULL;

    // If there are more commands
    if (tac.next != NULL)
    {
        // Copy them as well
        chain = new Tac(*tac.next);

        // Update chain
        this->next = chain;
        this->next->prev = this;
    }
}

Tac::~Tac()
{
    // Delete next instruction, if there is one
    if (this->next != NULL)
        delete this->next;
}

std::string Tac::getCodeString()
{
    std::stringstream code;

    // Add current code
    code << this->toString() << std::endl;

    // If there is more code
    if (this->next != NULL)
        // Conacetenate it as well
        code << this->next->getCodeString();

    // Return code
    return code.str();
}

std::string *Tac::getLabel()
{
    return this->label;
}

int Tac::getCodeSize()
{
    int instruction_count = 1; // Total instruction count (Starts at 1 to count this instruction as well)

    // Iterate every instruction while counting
    for (Tac *i = this->next; i != NULL; i = i->next, instruction_count++)
        ;

    // return total
    return instruction_count;
}

ILOCop Tac::getOpcode()
{
    return this->opcode;
}

Tac *Tac::getNext()
{
    return this->next;
}

void Tac::setLabel(std::string *label)
{
    this->label = label;
}

void Tac::addBefore(Tac *instruction)
{
    if (instruction != NULL)
    {
        // Add as the next instruction of this instruction's previous, if exists
        if (this->prev != NULL)
        {
            instruction->prev = this->prev;
            this->prev->next = instruction;
        }

        // Add this as instruction's next
        instruction->next = this;
        this->prev = instruction;
    }
}

void Tac::addAfter(Tac *instruction)
{
    if (instruction != NULL)
    {
        // Add as previous to next instruction, if there is any
        if (this->next != NULL)
        {
            this->next->prev = instruction;
            instruction->next = this->next;
        }

        // Add this instruction as previous
        this->next = instruction;
        instruction->prev = this;
    }
}

void Tac::addLast(Tac *instruction)
{
    Tac *aux = this;

    if (instruction != NULL)
    {
        // Iterate commands until last
        while (aux->next != NULL)
            aux = aux->next;

        // At the end, insert command
        aux->addAfter(instruction);
    }
}

std::string Tac::toString()
{
    std::stringstream code;

    // Add label if there is one
    if (this->label != NULL)
    {
        code << *this->label << ": " << std::endl;
    }

    // Add instruction name and a space
    code << "\t" << opname.at(this->opcode) << " ";

    // Based on instruction code
    switch (this->opcode)
    {
    // 0 operands
    case ILOC_NOP:
    case ILOC_HALT:
        break; // Do nothing
    // 1 operand
    case ILOC_JUMP:
    case ILOC_JUMPI:
        code << " => " << *this->arg1;
        break;
    // 2 operands
    case ILOC_LOAD:
    case ILOC_LOADI:
    case ILOC_STORE:
    case ILOC_I2I:
        code << *this->arg1 << " => " << *this->arg2;
        break;
    // 3 operands, format x => y, z
    case ILOC_STOREAI:
    case ILOC_STOREAO:
    case ILOC_CBR:
        code << *this->arg1 << " => " << *this->arg2 << ", " << *this->arg3;
        break;
    // 3 operands, format x, y => z
    default:
        code << *this->arg1 << ", " << *this->arg2 << " => " << *this->arg3;
        break;
    }

    // Return generated code string
    return code.str();
}

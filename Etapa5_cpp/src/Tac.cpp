#include "Tac.h"

// Register and label counters for name generation
uint Tac::register_count = 0;
uint Tac::label_count = 0;

std::string Tac::newRegister()
{
    std::stringstream new_register;

    // Make name
    new_register << "r" << Tac::register_count;

    // Update counter
    Tac::register_count++;

    return new_register.str();
}

std::string Tac::newLabel()
{
    std::stringstream new_label;

    // Make name
    new_label << "L" << Tac::label_count;

    // Update counter
    Tac::label_count++;

    return new_label.str();
}

Tac::Tac(ILOCop opcode_, std::string arg1_, std::string arg2_, std::string arg3_)
{
    this->opcode = opcode_;
    this->arg1 = arg1_;
    this->arg2 = arg2_;
    this->arg3 = arg3_;
    this->label = "";

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
    code << this->toString() << ";" << std::endl;

    // If there is more code
    if (this->next != NULL)
        // Conacetenate it as well
        code << this->next->getCodeString();

    // Return code
    return code.str();
}

std::string Tac::getLabel()
{
    return this->label;
}

void Tac::setLabel(std::string label)
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

    // Iterate commands until last
    while (aux->next != NULL)
        aux = aux->next;

    // At the end, insert command
    aux->addAfter(instruction);
}

std::string Tac::toString()
{
    std::stringstream code;

    // Add label if there is one
    if (!this->label.empty())
    {
        code << this->label << ": " << std::endl;
    }

    // Add instruction name and a space
    code << "\t" << opname.at(this->opcode) << " ";

    // Based on instruction code
    switch (this->opcode)
    {
    // 0 operands
    case ILOC_NOP:
        break; // Do nothing
    // 1 operand
    case ILOC_JUMP:
    case ILOC_JUMPI:
        code << this->arg1;
        break;
    // 2 operands
    case ILOC_LOAD:
    case ILOC_LOADI:
    case ILOC_STORE:
    case ILOC_I2I:
        code << this->arg1 << " => " << this->arg2;
        break;
    // 3 operands, format x => y, z
    case ILOC_STOREAI:
    case ILOC_STOREAO:
    case ILOC_CBR:
        code << this->arg1 << " => " << this->arg2 << ", " << this->arg3;
        break;
    // 3 operands, format x, y => z
    default:
        code << this->arg1 << ", " << this->arg2 << " => " << this->arg3;
        break;
    }

    // Return generated code string
    return code.str();
}

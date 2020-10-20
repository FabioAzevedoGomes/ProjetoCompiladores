#include "ILOC.h"

iloc_t *create_iloc_command(ILOCop opcode, void *arg1, void *arg2, void *arg3)
{
    // Allocate memoroy for command
    iloc_t *command = (iloc_t *)malloc(sizeof(iloc_t));

    // Fill data
    command->opcode = opcode;
    command->arg1 = arg1;
    command->arg2 = arg2;
    command->arg3 = arg3;

    // Return pointer to created command
    return command;
}

iloc_list_t *add_command(int index, iloc_list_t *first, iloc_t *command)
{
    iloc_list_t *aux = first;

    // Create entry for the command
    iloc_list_t *new_entry = (iloc_list_t *)malloc(sizeof(iloc_list_t *));
    new_entry->next = NULL;
    new_entry->prev = NULL;
    new_entry->command = command;

    // If inserting at the start
    if (index == 0)
    {
        // Update new entry pointer
        new_entry->next = first;

        // Update old first entry pointer
        first->prev = new_entry;

        // Return the inserted command
        return new_entry;
    }
    else if (aux == NULL)
    {
        // If list was null, return only new element
        return new_entry;
    }
    else
    {

        // Iterate elements until index or the end
        for (int i = 0; i < index && aux->next != NULL; i++, aux = aux->next)
            ;

        // When reached the end or the index, update pointers
        aux->prev->next = new_entry;
        new_entry->prev = aux->prev;
        aux->prev = new_entry;
        new_entry->next = aux;

        return first;
    }
}

void generate_code(node_t *root)
{
    // TODO Iterate tree, create iloc list and put in root node
}

void output_code(iloc_list_t *first)
{
    iloc_list_t *aux;

    // Iterate list
    for (aux = first; aux != NULL; aux = aux->next)
    {
        // Based on opcode
        switch (first->command->opcode)
        {
        case ILOC_ADD: // TODO
        case ILOC_SUB:
        case ILOC_MULT:
        case ILOC_DIV:
        case ILOC_ADDI:
        case ILOC_SUBI:
        case ILOC_RSUBI:
        case ILOC_MULTI:
        case ILOC_DIVI:
        case ILOC_RDIVI:
        case ILOC_LSHIFT:
        case ILOC_LSHIFTI:
        case ILOC_RSHIFT:
        case ILOC_RSHIFTI:
        case ILOC_LOAD:
        case ILOC_LOADI:
        case ILOC_LOADAI:
        case ILOC_LOADA0:
        case ILOC_STORE:
        case ILOC_STOREAI:
        case ILOC_STOREAO:
        case ILOC_I2I:
        case ILOC_CMP_LT:
        case ILOC_CMP_L3:
        case ILOC_CMP_EQ:
        case ILOC_CMP_GE:
        case ILOC_CMP_GT:
        case ILOC_CMP_NE:
        case ILOC_CBR:
        case ILOC_JUMPI:
        case ILOC_JUMP:
        default: // Unkown command
            printf("nop");
            break;
        }

        // Newline
        printf("\n");
    }
}

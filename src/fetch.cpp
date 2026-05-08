#include <stdio.h>
#include "../header/fetch.h"

//int Fetch(FILE *file, IF_ID_buffer *if_id_buf, bool debug)
int Fetch(const char* file_name, IF_ID_buffer *if_id_buf, bool debug)
{
    
    if (debug)
    {
        if (pipeline)
        {
            static int inst_index = 0;
            if (!STALL)
            inst_index++;
            if_id_buf->instr_index = inst_index;
            printf("\nFETCH STAGE (%d) ===============================\n", inst_index);
        }
        else
            printf("\nFETCH STAGE ===============================\n");
    }
    // We open file per fetch so that the buffer refills
    FILE* file = fopen(file_name, "r");

    /*
        Fetch reads an input file of machine instruction lines.
        Each line is a 32-bit instruction represented as a string of 1s and 0s.
        We read in the instruction as a string and store it in the input buffer
        along with the current program counter value for use in the decode stage.

        Each line is enumerated as an instruction read using the pc value. 
        For example, if pc is 0, we read in the 0th instruction, 
        if pc is 4, we read in the 1st instruction, etc.
    */

    int result = 0;
    if (STALL)
    {
        if (debug)
            printf("STALLING...\n");
        return result; // return to prevent buffer update
    }

    // We update pc value based on the value of PCSrc
    if (debug)
        printf("PCSrc = %d\n", PCSrc);
    switch(PCSrc)
    {
        case 1: // Branch Target (PC + immediate if condition)
            if (alu_zero)
            {
                if (debug)
                    std::cout << "Branch taken, updating program counter to branch target address " << branch_target << std::endl;
                pc = branch_target;
                break;
            }
            // fall out to default
        case 2: // Jal (PC + immediate)
            if (debug)
                std::cout << "Jumping (Jal) to " << jal_target << std::endl;
            pc = jal_target;
            break;
        case 3: // Jalr (rs1 + immediate)
            if (debug)
                std::cout << "Jumping (Jalr) to " << jalr_target << std::endl;
            pc = jalr_target;
            break;
        default: // Normal
            if (debug)
                std::cout << "Taking normal PC + 4 = " << next_pc << std::endl;
            pc = next_pc;
            break;
    }
    if (pc > 0)
    printf("pc is modified to 0x%x\n", pc);

    if (debug)
        printf("Reading instruction at PC = %d\n", pc);

    // get line number to read based on current pc value
    int line = pc / 4; 

    // read until first whitespace, store in instruction field of IF/ID buffer
    for (int i = 0; i < line; i++)
    {
        // skip lines until we get to the line we want to read
        fscanf(file, "%*[^\n]\n");
    }
    
    // read up to 32 chars or whitespace, store in instruction field of IF/ID buffer
    result = fscanf(file, "%32s", if_id_buf->instruction); // read the instruction into the buffer

    // reset PCSrc
    PCSrc = 0;
    // if we stalled, then nothing is read and the buffer gets no update

    // No matter what we chose, increment default next pc +4 after current position
    next_pc = pc + 4;
    // store the (normally expected [pc + 4]) next pc value in the buffer 
    if_id_buf->pc = next_pc;
    if (debug)
        std::cout << "PC incremented." << std::endl;

    if (FLUSH == 1)
    {
        // no need to flush the instruction string as the decode stage has done so if necessary
        // since this technically happens last in the iteration.
        // Eg. fetch has not gotten an out of date instruction before any changes to PC
        FLUSH--;
    }

    if (debug)
    {
        if_id_buf->print_buffer();
        printf("============================================\n\n");
    }

    // fscanf returns the number of items successfully read, 
    // so it will return 0 if we have reached the end of the input stream 
    // and there are no more instructions to read.
    return result;
}
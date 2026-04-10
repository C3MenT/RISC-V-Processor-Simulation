#include <iostream>
#include "../header/fetch.h"
#include "../header/datapath.h"

// Debug mode flag
bool debug = true;

// Main Driver Function ====================================================================
/*
    Fetch - Must fetch sequential instructions from input file until end of input stream

    Decode - Takes fetched instruction and decodes it to determine the instruction type and 
        relevant fields:
        Format: <instruction name> <rd>, <rs1>, <rs2/imm>
        It must additionally return the values that are currently in the registers specified
        or the immediate value if applicable.

    Execute - Execute the instruction by performing the relevant operation on the register values 
        or immediate value and store the result in the appropritate buffer register if applicable.

    Memory - If the instruction is a load or store, perform the relevant memory operation by 
        reading/writing to the data memory

    Write Back - If the instruction is a R-type, I-type, or U-type, write the result of the instruction 
        back to the register file.

    Since this is pipelined, we must call each stage function as if they are happening simultaneously.
    Therefore, they should have no vision into other stages and only operate on what is in the input buffers
    each call, writing to their respective output buffer for the next stage to read from.
*/
int main(int argc, char* argv[])
{
    // Open the input file containing the machine code instructions
    FILE* file = fopen(argv[1], "r");

    IF_ID_buffer if_id_buffer; // input buffer for the decode stage
    ID_EXE_buffer id_exe_buffer; // output buffer for the decode stage and input

    // Fetch the instruction
    while (Fetch(file, &if_id_buffer) > 0) // while we are still reading instructions
    {        
        // Decode the fetched instruction
        Decode(reg_file, if_id_buffer, id_exe_buffer, control_signals, debug);

    //PC += 4; // Increment the program counter by 4 for the next instruction
    }

    delete if_id_buffer; delete id_exe_buffer;
    return 0;
}
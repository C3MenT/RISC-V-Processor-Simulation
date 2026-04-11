#include <iostream>
#include "../header/datapath.h"
#include "../header/fetch.h"
#include "../header/decoder.h"

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

    // Check if the file was opened successfully
    if (argv[1] == nullptr)
    {
        std::cerr << "Usage: ./riscv_simulator <input_file>" << std::endl;
        return 1;
    }

    // Declare the buffers
    IF_ID_buffer if_id_buffer; // input buffer for the decode stage
    ID_EXE_buffer id_exe_buffer; // output buffer for the decode stage and input
    EXE_MEM_buffer exe_mem_buffer; // output buffer for the execute stage and input for the memory stage
    MEM_WB_buffer mem_wb_buffer; // output buffer for the memory stage and input

    // Loose control signals to be set
    pc = 0; // Initialize the pc
    alu_zero = 0; // Initialize the alu_zero flag to 0

    // Initialize the register file and control signals to 0
    for (int i = 0; i < 32; i++)
    {
        rf[i] = 0;
    }
    for (int i = 0; i < 7; i++)
    {
        control_signals[i] = 0;
    }

    int cycle = 0; // keep track of cycle number for debug output

    // Fetch the instruction
    while (Fetch(file, &if_id_buffer) > 0) // while we are still reading instructions
    {        
        cycle++; // increment cycle number
        if (debug)
        {std::cout << "Cycle " << cycle << std::endl;}

        // Decode the fetched instruction
        Decode(rf, &if_id_buffer, &id_exe_buffer, control_signals, debug);



        // End debug report for the cycle
        if (debug)
        {

            std::cout << "Register File: ";
            for (int i = 0; i < 32; i++)
            {
                std::cout << "x" << i << ": " << rf[i] << " ";
                if (i % 8 == 7)
                {std::cout << std::endl;}
            }
            std::cout << std::endl;

            std::cout << "Control Signals: " << std::endl;
            std::cout << "RegWrite: " << control_signals[0] << " " << std::endl;
            std::cout << "Branch: " << control_signals[1] << " " << std::endl;
            std::cout << "ALUSrc: " << control_signals[2] << " " << std::endl;
            std::cout << "MemWrite: " << control_signals[3] << " " << std::endl;
            std::cout << "MemtoReg: " << control_signals[4] << " " << std::endl;
            std::cout << "MemRead: " << control_signals[5] << " " << std::endl;
            std::cout << "ALUOp: " << control_signals[6] << " " << std::endl;

            std::cout << "ALU Zero Flag: " << alu_zero << " " << std::endl;
        }
    }

    return 0;
}
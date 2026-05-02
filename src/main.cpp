#include <iostream>
#include "../header/datapath.h"
#include "../header/fetch.h"
#include "../header/decoder.h"
#include "../header/execute.h"
#include "../header/mem.h"
#include "../header/writeback.h"

// Debug mode flag
bool debug = false;

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
    FILE* file;
    
    printf("args: %d\n", argc);

    // Check if main was given a file name as a command line argument 
    if (argv[1] == nullptr)
    {
        char input_file_name[100]; // buffer size is arbitrary
        // if not, prompt the user to enter one like instructions say
        printf("Enter the program file name to run: \n");
        std::fscanf(stdin, "%s", input_file_name);
        file = fopen(input_file_name, "r");
        if (file == nullptr)
        {
            std::cerr << "Error: Could not open file \"" << input_file_name << "\"" << std::endl;
            return 1;
        }
    }
    else
    {
        file = fopen(argv[1], "r");
        if (file == nullptr)
        {
            std::cerr << "Error: Could not open file \"" << argv[1] << "\"" << std::endl;
            return 1;
        }
        // Additionally allow debug toggling from command prompt
        if (argv[2] != nullptr && argv[2][0] == '-' && argv[2][1] == 'd')
        {
            printf("Debug mode enabled\n");
            debug = true;
        }
    }

    // Declare the buffers //
    // output buffer for the fetch stage and input buffer for the decode stage
    IF_ID_buffer if_id_buffer; 
    // output buffer for the decode stage and input for the execute stage
    ID_EXE_buffer id_exe_buffer; 
    // output buffer for the execute stage and input for the memory stage
    EXE_MEM_buffer exe_mem_buffer; 
    // output buffer for the memory stage and input for the write back stage
    MEM_WB_buffer mem_wb_buffer; 

    // Loose control signals to be set
    pc = 0; // Initialize the pc
    alu_zero = 0; // Initialize the alu_zero flag to 0

    // Initialize the register file and
    // Initialize the data memory to 0
    for (int i = 0; i < 32; i++)
    {
        rf[i] = 0;
        d_mem[i] = 0;
    }

    int cycle = 0; // keep track of cycle number for debug output

    // Test dependent initializations

    // (Sample Part 1)
    //rf[1] = 32; rf[2] = 5; rf[10] = 112; rf[11] = 4;
    //d_mem[28] = 5; d_mem[29] = 16;

    // (Sample Part 2)
    //rf[8] = 32; rf[10] = 5; rf[11] = 2; rf[12] = 10; rf[13] = 15;

    // Main simulation loop: Fetch, Decode, Execute, Memory, Write Back
    
    /*
    // We execute the stages in reverse order to simulate the pipelining, so we call write back first and fetch last.
    // This is literally the case as the stages are happening simultaneously, so later ones would finish earlier in the code.
    // This also incidentally prevents using buffer values intended for future cycles in the current cycle, which would be incorrect.
    do
    {   
        if (debug)
        {std::cout << std::endl << "Cycle " << cycle << std::endl << std::endl;;}

        // Writeback Stage
        Writeback(&mem_wb_buffer, debug);

        // Memory Stage
        Mem(&exe_mem_buffer, &mem_wb_buffer, debug);

        // Execute Stage
        Execute(&id_exe_buffer, &exe_mem_buffer, alu_ctrl, debug);

        // Decode the fetched instruction
        // `Decode` reads/writes global `control_signals`, so pass only debug flag
        Decode(rf, &if_id_buffer, &id_exe_buffer, debug);

        // End debug report for the cycle
        if (debug)
        {
            std::cout << std::endl;
            std::cout << "Register File: " << std::endl;
            for (int i = 0; i < 32; i++)
            {
                std::cout << "x" << i << ": " << rf[i] << " ";
                if (i % 8 == 7)
                {std::cout << std::endl;}
            }
            std::cout << std::endl;

            std::cout << "Control Signals: " << std::endl;
            //std::cout << "RegWrite: " << control_signals[0] << " " << std::endl;
            std::cout << "RegWrite: " << RegWrite << " " << std::endl;
            std::cout << "Branch: " << Branch << " " << std::endl;
            std::cout << "ALUSrc: " << ALUSrc << " " << std::endl;
            std::cout << "MemWrite: " << MemWrite << " " << std::endl;
            std::cout << "MemtoReg: " << MemtoReg << " " << std::endl;
            std::cout << "MemRead: " << MemRead << " " << std::endl;
            std::cout << "ALUOp: " << ALUOp[0] << ALUOp[1] << " " << std::endl;

            std::cout << "ALU Zero Flag: " << alu_zero << " " << std::endl;
            std::cout << "======================================================" << std::endl << std::endl;
        }
        cycle++; // increment cycle number
    // Fetch the instruction
    } while ((Fetch(file, &if_id_buffer, debug) > 0) && (total_clock_cycles > cycle + 4)); // while we are still reading instructions and last is incomplete
    */

    // Sequential Implementation
    while (Fetch(file, &if_id_buffer, debug) > 0)
    {
        cycle++;

        printf("\ntotal_clock_cycles %d:\n", cycle);
    
        Decode(rf, &if_id_buffer, &id_exe_buffer, debug);

        Execute(&id_exe_buffer, &exe_mem_buffer, alu_ctrl, debug);

        Mem(&exe_mem_buffer, &mem_wb_buffer, debug);
        if (MemWrite)
            printf("memory 0x%x is modified to 0x%x\n", exe_mem_buffer.alu_result, exe_mem_buffer.rs2_val);

        Writeback(&mem_wb_buffer, debug);
        if (!Branch)
        {
            if (MemRead)
                printf("x%d is modified to 0x%x\n", mem_wb_buffer.rd, mem_wb_buffer.mem_result);
            else if (RegWrite)
                printf("x%d is modified to 0x%x\n", mem_wb_buffer.rd, mem_wb_buffer.alu_result);
        }

        printf("pc is modified to 0x%x\n", pc);
    }
    printf("\nprogram terminated:\ntotal execution time is %d cycles\n", total_clock_cycles);
    return 0;
}
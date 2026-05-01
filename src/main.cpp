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
    
    /*
    // Open the input file containing the machine code instructions
    FILE* file = fopen(argv[1], "r");
    
    // Check if the file was opened successfully
    if (argv[1] == nullptr)
    {
        std::cerr << "Usage: ./riscv_simulator <input_file>" << std::endl;
        return 1;
    }
    */
    // For testing purposes, we will read from standard input instead of a file
    printf("Enter the program file name to run: \n");
    char input_file[100];
    std::fscanf(stdin, "%s", input_file);
    FILE* file = fopen(input_file, "r");

    if (file == nullptr)
    {
        std::cerr << "Error: Could not open file " << input_file << std::endl;
        return 1;
    }
    printf("Running program from file: %s\n", input_file);
    // Declare the buffers //
    // input buffer for the decode stage
    IF_ID_buffer if_id_buffer; 
    if_id_buffer.pc = 0; // initialize the pc value in the IF/ID buffer to 0
    // initialize the instruction field in the IF/ID buffer to all 0s
    if_id_buffer.instruction[0] = '\0';
    
    // output buffer for the decode stage and input
    ID_EXE_buffer id_exe_buffer; 
    id_exe_buffer.pc = 0; // initialize pc value in ID/EXE buffer to 0
    id_exe_buffer.read_data1 = 0; // initialize read data 1 value
    id_exe_buffer.read_data2 = 0; // initialize read data 2 value
    id_exe_buffer.immediate = 0; // initialize immediate value
    id_exe_buffer.rs1 = 0; // initialize rs1 value
    id_exe_buffer.rs2 = 0; // initialize rs2 value
    id_exe_buffer.rd = 0; // initialize rd value

    EXE_MEM_buffer exe_mem_buffer; // output buffer for the execute stage and input for the memory stage
    exe_mem_buffer.pc = 0; // initialize pc value in EXE/MEM buffer to 0
    exe_mem_buffer.alu_result = 0; // initialize alu result value
    exe_mem_buffer.rs1_val = 0; // initialize rs1 value for store and load instructions
    exe_mem_buffer.rs2_val = 0; // initialize rs2 value for store instructions
    exe_mem_buffer.rd = 0; // initialize rd reg number

    MEM_WB_buffer mem_wb_buffer; // output buffer for the memory stage and input
    mem_wb_buffer.mem_result = 0; // initialize memory result value
    mem_wb_buffer.alu_result = 0; // initialize alu result value
    mem_wb_buffer.rd = 0; // initialize rd reg name

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
    /*for (int i = 0; i < 7; i++)
    {
        control_signals[i] = 0;
    }*/
    // Initialize control signals to 0
    RegWrite = 0;
    Branch = 0;
    ALUSrc = 0;
    MemWrite = 0;
    MemtoReg = 0;
    MemRead = 0;
    Jump = 0;
    ALUOp[0] = 0;
    ALUOp[1] = 0;

    int cycle = 0; // keep track of cycle number for debug output

    // Test dependent initializations (Sample Part 1)
    //rf[1] = 32; rf[2] = 5; rf[10] = 112; rf[11] = 4;
    //d_mem[28] = 5; d_mem[29] = 16;
    // Test dependent initializations (Sample Part 2)
    rf[8] = 32; rf[10] = 5; rf[11] = 2; rf[12] = 10; rf[13] = 15;


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
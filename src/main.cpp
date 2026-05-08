#include <iostream>
#include "../header/datapath.h"
#include "../header/fetch.h"
#include "../header/decoder.h"
#include "../header/execute.h"
#include "../header/mem.h"
#include "../header/writeback.h"

// Debug mode flag
bool debug = false;

// Index corresponds to reg index
// string corresponds to reg name
const char* reg_map[32] = {
    "zero", "ra", "sp", "gp", "tp",
    "t0", "t1", "t2", 
    "s0", "s1",
    "a0", "a1",
    "a2", "a3", "a4", "a5", "a6", "a7",
    "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11",
    "t3", "t4", "t5", "t6"
};

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
    // File object pointer to hold file
    FILE* file;
    const char* file_name;

    // // Check if main was given a file name as a command line argument // //
    // Using runtime input
    if (argv[1] == nullptr)
    {
        char input_file_name[100]; // buffer size is arbitrary
        // if not, prompt the user to enter one like instructions say
        printf("Enter the program file name to run: \n");
        std::fscanf(stdin, "%s", input_file_name);
        // Open the input file containing the machine code instructions
        file = fopen(input_file_name, "r");
        if (file == nullptr)
        {
            std::cerr << "Error: Could not open file \"" << input_file_name << "\"" << std::endl;
            return 1;
        }
        file_name = input_file_name;
    }
    // Using Command Line
    else
    {
        // Open the input file containing the machine code instructions
        file = fopen(argv[1], "r");
        if (file == nullptr)
        {
            std::cerr << "Error: Could not open file \"" << argv[1] << "\"" << std::endl;
            return 1;
        }
        // Additionally allow debug toggling from command prompt
        if ((argv[2] != nullptr && argv[2][0] == '-' && argv[2][1] == 'd') || 
        (argv[3] != nullptr && argv[3][0] == '-' && argv[3][1] == 'd') )
        {
            printf("Debug mode enabled\n");
            debug = true;
        }
        // Additionally allow pipeline toggling
        if ((argv[2] != nullptr && argv[2][0] == '-' && argv[2][1] == 'p') || 
        (argv[3] != nullptr && argv[3][0] == '-' && argv[3][1] == 'p') )
        {
            pipeline = true;
        }

        file_name = argv[1];
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
    alu_zero = 0; // Initialize the alu_zero flag to 0 (left over from using globals)

    // Initialize the register file and
    // Initialize the data memory to 0
    for (int i = 0; i < 32; i++)
    {
        rf[i] = 0;
        d_mem[i] = 0;
    }

    int cycle = 0; // keep track of cycle number for debug output
    total_clock_cycles = 0; // keep track of total clock cycles

    // // Test dependent initializations // //
    // (Sample Part 1)
    if (string_comp(file_name, "tests/sample_part1.txt"))
    {
        rf[1] = 32; rf[2] = 5; rf[10] = 112; rf[11] = 4;
        d_mem[28] = 5; d_mem[29] = 16;
    }

    // (Sample Part 2)
    if (string_comp(file_name, "tests/sample_part2.txt"))
    {rf[8] = 32; rf[10] = 5; rf[11] = 2; rf[12] = 10; rf[13] = 15;}

    // (Pipelined Load Test)
    //rf[6] = 1; rf[8] = 32; d_mem[32] = 10;



    // ========== // MAIN DRIVER // ========== //
    // Main simulation loops: Fetch, Decode, Execute, Memory, Write Back
    // Sequential default, with option to run pipelined through flag
    if (!pipeline)
    {
        // Sequential (Single-Cycle) Implementation
        // Since buffers are updated and then immediately passed to next stage, this is single-cycle
        // A pipelined implementation would require each stage to operate on their own instance of
        // the buffers so that they can be updated simultaneously without interfering with each other.)
        while (Fetch(file_name, &if_id_buffer, debug) > 0)
        {
            cycle++;

            printf("\ntotal_clock_cycles %d:\n", cycle);
        
            Decode(&if_id_buffer, &id_exe_buffer, debug);

            Execute(&id_exe_buffer, &exe_mem_buffer, alu_ctrl, debug);

            Mem(&exe_mem_buffer, &mem_wb_buffer, debug);
            if (exe_mem_buffer.MemWrite)
                printf("memory 0x%x is modified to 0x%x\n", exe_mem_buffer.alu_result, exe_mem_buffer.rs2_val);
            
            Writeback(&mem_wb_buffer, debug);
            if (!exe_mem_buffer.MemWrite)
            {
                if (use_reg_names)
                {
                    printf("%s is modified to 0x%x\n", reg_map[mem_wb_buffer.rd], rf[mem_wb_buffer.rd]);
                }
                else
                {
                    printf("x%d is modified to 0x%x\n", mem_wb_buffer.rd, rf[mem_wb_buffer.rd]);
                }
            }
        }
    }
    else
    {
        // Pipelined Implementation
        // We execute the stages in reverse order to simulate the pipelining, so we call write back first and fetch last.
        // Conceptually this is the case as the stages are happening simultaneously so later ones would finish earlier in the code.
        // This also incidentally prevents using buffer values intended for future cycles in the current cycle (loop iteration), which would be incorrect.
        
        // The main function still contains an objective instance of each buffer.
        // Each stage will take in its input and output buffers like normal
        // but, instead of passing the same buffer instance to the next stage,
        // they wait until the next cycle for the stage to "get it themselves."
        // This simulates the fact that in a pipelined implementation, each stage would have its own instance of the buffer registers that get updated
        // simultaneously at the end of each cycle. 

        // We also need to add a condition to the loop to ensure that we run enough cycles to complete the last few instructions that are 
        // still in the pipeline after we finish fetching all instructions from the input file.
        // Recall Cycles = Instructions + Pipeline Depth (5) - 1, so we need to run at least 4 additional cycles after the last instruction
        // is fetched to allow it to fully propagate through the 5-stage pipeline and complete execution.

        // Count the instructions in the input file for later use in pipelined implementation
        int instruction_count = 0;
        char s[33];
        while (fscanf(file, "%32s", s) > 0)
        {instruction_count++;}
        if (debug)
            printf("%d Instructions\n", instruction_count);
        // since writeback will assume next instr is pc+4 by default it always adds 4 by default
        // we subtract 4 here to account for that
        pc -= 4; 
        // subtract 1 from total cycles before the loop begins as each loop starts by incrementing
        total_clock_cycles--;
        do
        {   
            if (total_clock_cycles > -1)
                printf("\ntotal_clock_cycles %d:\n", total_clock_cycles + 1);

            Writeback(&mem_wb_buffer, debug);
            if (mem_wb_buffer.RegWrite && total_clock_cycles < instruction_count + 4)
            {
                if (use_reg_names)
                {
                    printf("%s is modified to 0x%x\n", reg_map[mem_wb_buffer.rd], rf[mem_wb_buffer.rd]);
                }
                else
                {
                    printf("x%d is modified to 0x%x\n", mem_wb_buffer.rd, rf[mem_wb_buffer.rd]);
                }
            }
        
            Mem(&exe_mem_buffer, &mem_wb_buffer, debug);
            
            if (exe_mem_buffer.MemWrite && total_clock_cycles < instruction_count + 4)
                printf("memory 0x%x is modified to 0x%x\n", exe_mem_buffer.alu_result, exe_mem_buffer.rs2_val);

            Execute(&id_exe_buffer, &exe_mem_buffer, alu_ctrl, debug);
 
            Decode(&if_id_buffer, &id_exe_buffer, debug, &exe_mem_buffer, &mem_wb_buffer);

        // Fetch the next instruction and process loop while we are still reading instructions or last is incomplete
        } while ((Fetch(file_name, &if_id_buffer, debug) > 0) || (total_clock_cycles < instruction_count + 4));    
    }
    printf("\nprogram terminated:\ntotal execution time is %d cycles\n", total_clock_cycles);
    return 0;
}
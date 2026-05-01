#include <iostream>
#include "../header/mem.h"


void Mem(EXE_MEM_buffer *exe_mem_buffer, MEM_WB_buffer *mem_wb_buffer, bool debug){
    mem_wb_buffer->alu_result = exe_mem_buffer->alu_result; // pass the effective address to the mem/wb buffer for use in the write back stage for store instructions
    mem_wb_buffer->pc = exe_mem_buffer->pc; // pass the pc value to the mem/wb buffer for use in the write back stage for branch instructions

    int address = exe_mem_buffer->alu_result; 
    int index = (address)/4; // converted for int array
    int value = d_mem[index]; //dmem is declared with the global variables

    if (debug)
    {
        std::cout << "MEMORY STAGE ===============================\n";
        if (MemRead)
        {
            std::cout << "Reading from memory address " << address << " to get value " << value << std::endl;
        }
        else if (MemWrite)
        {
            std::cout << "Writing value " << exe_mem_buffer->rs2_val << " to memory address " << address << std::endl;
            
        }
        else
        {
            std::cout << "No memory operation" << std::endl;
        }
        std::cout << "============================================\n" << std::endl;
    }

    if(MemRead) //this means its load instruction
    {
        mem_wb_buffer->mem_result = value;   //store the value in the mem/wb buffer
    }
    if(MemWrite) //this means its store word
    {
        d_mem[index] = exe_mem_buffer->rs2_val; // rs2 would hold the value to store for store instructions, so we write that value to memory at the effective address
    }
    mem_wb_buffer->rd = exe_mem_buffer->rd;  //pass the destination register to the mem/wb buffer
}
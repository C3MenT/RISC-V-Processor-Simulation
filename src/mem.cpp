#include <iostream>
#include "../header/mem.h"


void Mem(EXE_MEM_buffer *exe_mem_buffer, MEM_WB_buffer *mem_wb_buffer, bool debug){
    if (FLUSH == 4) // if we must flush up to EXE/MEM buffer
    {
        exe_mem_buffer->rd = 0;
        exe_mem_buffer->alu_result = 0;
        exe_mem_buffer->rs1_val = 0;
        exe_mem_buffer->rs2_val = 0;
        exe_mem_buffer->ALU_Zero = 0;
        exe_mem_buffer->Branch = 0;
        exe_mem_buffer->Jump = 0;
        exe_mem_buffer->MemRead = 0;
        exe_mem_buffer->MemtoReg = 0;
        exe_mem_buffer->MemWrite = 0;
        exe_mem_buffer->RegWrite = 0;
        FLUSH--;
    }
    
    mem_wb_buffer->alu_result = exe_mem_buffer->alu_result; // pass the effective address to the mem/wb buffer for use in the write back stage for store instructions
    mem_wb_buffer->pc = exe_mem_buffer->pc; // pass the pc value to the mem/wb buffer for use in the write back stage for branch instructions
    mem_wb_buffer->pc_target = exe_mem_buffer->pc_target;

    mem_wb_buffer->RegWrite = exe_mem_buffer->RegWrite;
    mem_wb_buffer->MemtoReg = exe_mem_buffer->MemtoReg;
    mem_wb_buffer->Branch = exe_mem_buffer->Branch;
    mem_wb_buffer->Jump = exe_mem_buffer->Jump;
    mem_wb_buffer->ALU_Zero = exe_mem_buffer->ALU_Zero;
    mem_wb_buffer->PCSrc = exe_mem_buffer->PCSrc;

    int address = exe_mem_buffer->alu_result; // if this is a mem instruction the alu_result is the address
    int index = (address)/4; // converted for int array
    int value = d_mem[index]; //dmem is declared with the global variables

    if (debug)
    {
        if (pipeline)
        {
            static int inst_index = -3;
            inst_index++;
            printf("\nMEMORY STAGE (%d) ===============================\n", inst_index);
        }
        else
            printf("\nMEMORY STAGE ===============================\n");

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

    if(debug)
    {
        mem_wb_buffer->print_buffer();
        std::cout << "============================================\n" << std::endl;
    }
}
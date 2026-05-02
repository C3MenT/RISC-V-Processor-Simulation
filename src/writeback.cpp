#include <iostream>
#include "../header/writeback.h"

void Writeback(MEM_WB_buffer *mem_wb_buffer, bool debug){
    if (debug)
    {
        std::cout << "\nWRITEBACK STAGE ===============================\n";
        if (RegWrite)
        {
            std::cout << "Writing back to register file: ";
            if (MemtoReg)
            {
                std::cout << "Memory result " << mem_wb_buffer->mem_result << " to x" << mem_wb_buffer->rd << std::endl;
            }
            else
            {
                std::cout << "ALU result " << mem_wb_buffer->alu_result << " to x" << mem_wb_buffer->rd << std::endl;
            }
        }
        else
        {
            std::cout << "No write back to register file" << std::endl;
        }
        std::cout << "============================================\n" << std::endl;
    }

    if(MemtoReg && RegWrite && mem_wb_buffer->rd!=0)
    {
        rf[mem_wb_buffer->rd] = mem_wb_buffer->mem_result;
    }
    else if (MemtoReg == 0 && RegWrite && mem_wb_buffer->rd!=0){
        rf[mem_wb_buffer->rd] = mem_wb_buffer->alu_result;
    }

    total_clock_cycles+=1;
    if (Branch && alu_zero)
    {
        pc = branch_target; // update program counter to branch target address if we are taking the branch
        if (debug)
        {
            std::cout << "Branch taken, updating program counter to branch target address " << branch_target << std::endl;
        }
    }
    else if (Jump)
    {
        pc = branch_target;
        rf[mem_wb_buffer->rd] = mem_wb_buffer->pc; // write the return address to the destination register for jump instructions
        
        if (debug)
        {
            std::cout << "Jumping to " << branch_target << std::endl; 
        }
    }
    else
    {
        pc += 4; // increment program counter by 4 to point to the next instruction
    }
}
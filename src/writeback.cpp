#include <iostream>
#include "../header/writeback.h"

void Writeback(MEM_WB_buffer *mem_wb_buffer, bool debug){
    if (debug)
    {
        if (pipeline)
        {
            //static int inst_index = -4;
            printf("\nWRITEBACK STAGE (%d) ===============================\n", mem_wb_buffer->instr_index);
        }
        else
            printf("\nWRITEBACK STAGE ===============================\n");
    }
    // Writing Something
    if (mem_wb_buffer->RegWrite)
    {
        if (debug)
            std::cout << "Writing back to register file: ";
        // Jump Case
        if (mem_wb_buffer->Jump)
        {
            /*
            if (!pipeline)
            {
                if (mem_wb_buffer->PCSrc == 2) // JAL
                {
                    pc = mem_wb_buffer->pc_target; // pc = pc + immediate (found by Decode "Adder")
                }
                else // JALR
                {
                    pc = mem_wb_buffer->alu_result; // pc = rs1 + immediate (found in ALU)
                }
                if (debug)
                    std::cout << "Jumping to " << pc << std::endl;
            }
            */
            rf[mem_wb_buffer->rd] = mem_wb_buffer->pc; // write pc+4 value into rd
            if (debug)
            {
                std::cout << "PC + 4 = " << mem_wb_buffer->pc << " to x" << mem_wb_buffer->rd << std::endl;
            }
        }
        // Normal Write
        else 
        {
            // Load Case
            if(mem_wb_buffer->MemtoReg)
            {
                if (debug)
                    std::cout << "Memory result " << mem_wb_buffer->mem_result << " to x" << mem_wb_buffer->rd << std::endl;
                rf[mem_wb_buffer->rd] = mem_wb_buffer->mem_result;
            }
            // ALU Operation Case
            else 
            {
                if (debug)
                    std::cout << "ALU result " << mem_wb_buffer->alu_result << " to x" << mem_wb_buffer->rd << std::endl;
                rf[mem_wb_buffer->rd] = mem_wb_buffer->alu_result;
            }
            /*
            pc += 4; // increment program counter by 4 to point to the next instruction
            if (debug)
                std::cout << "PC incremented normally." << std::endl;
            */
        }
    }
    // Not Writing Anything
    else 
    {
        if(debug)
            std::cout << "No write back to register file" << std::endl;
        /*
        // Branch Case
        if (mem_wb_buffer->Branch && mem_wb_buffer->ALU_Zero && !pipeline)
        {
            pc = mem_wb_buffer->pc_target; // pc = pc + immediate (found by Decode "Adder")
            if (debug)
                std::cout << "Branch taken, updating program counter to branch target address " << mem_wb_buffer->pc_target << std::endl;
        }
        else // We are doing an instruction that does not branch or jump anywhere or write anything...
        {
            pc += 4; // increment program counter by 4 to point to the next instruction just in case
            if (debug)
                std::cout << "PC incremented normally." << std::endl;
        }
        */
    }

    rf[0] = 0; // Hard Reset the Zero Register to 0
    if (!STALL)
    total_clock_cycles++; // Project instructions imply total clock cycles be updated in write back

    if (debug)
        std::cout << "============================================\n" << std::endl;
}
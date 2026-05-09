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
        }
    }
    // Not Writing Anything
    else 
    {
        if(debug)
            std::cout << "No write back to register file" << std::endl;
    }

    rf[0] = 0; // Hard Reset the Zero Register to 0
    
    total_clock_cycles++; // Project instructions imply total clock cycles be updated in write back

    if (debug)
        std::cout << "============================================\n" << std::endl;
}
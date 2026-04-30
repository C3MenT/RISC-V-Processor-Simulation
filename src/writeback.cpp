#include <iostream>
#include "../header/writeback.h"

void Writeback(MEM_WB_buffer *mem_wb_buffer, bool debug){
    if (debug)
    {
        std::cout << "WRITEBACK STAGE ===============================\n";
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
        std::cout << "============================================\n";
    }

    if(MemtoReg && RegWrite && mem_wb_buffer->rd!=0)
    {
        rf[mem_wb_buffer->rd]= mem_wb_buffer->mem_result;
    }
    else if (MemtoReg==0 && RegWrite && mem_wb_buffer->rd!=0){
        rf[mem_wb_buffer->rd]= mem_wb_buffer->alu_result;
    }

    total_clock_cycles+=1;
}
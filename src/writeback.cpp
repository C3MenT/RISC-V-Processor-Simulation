#include <iostream>
#include "../header/writeback.h"

void Writeback(MEM_WB_buffer *mem_wb_buffer){
    if(control_signals[4]==1 && control_signals[0]==1 && mem_wb_buffer->rd!=0)
    {
        rf[mem_wb_buffer->rd]= mem_wb_buffer->mem_result;
    }
    else if (control_signals[4]==0 && control_signals[0]==1 && mem_wb_buffer->rd!=0){
        rf[mem_wb_buffer->rd]= mem_wb_buffer->alu_result;
    }

    total_clock_cycles+=1;
}
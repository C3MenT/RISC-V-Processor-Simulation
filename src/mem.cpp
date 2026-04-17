#include <iostream>
#include "../header/mem.h"


void mem(EXE_MEM_buffer *exe_mem_buffer, MEM_WB_buffer *mem_wb_buffer, int address){
    int index= address/4;
    int value = d_mem[index];   //dmem is declared with the global variables
    if(control_signals[5]==1) //this means its load instruction
    {
        mem_wb_buffer->mem_result= value;   //store the value in the mem/wb buffer
        mem_wb_buffer->rd= exe_mem_buffer->rd;  //pass the destination register to the mem/wb buffer
    }
    if(control_signals[3]==1) //this means its store word
    {
        d_mem[index]= exe_mem_buffer->rs2_val; //
    }
}
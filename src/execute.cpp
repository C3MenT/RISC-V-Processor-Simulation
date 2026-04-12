#include <iostream>
#include "../header/execute.h"

// Implementation for the execute stage
void execute(ID_EXE_buffer *id_exe_buffer, EXE_MEM_buffer *exe_mem_buffer, int alu_ctrl[], bool debug = false)
{
    if (debug)
    {
        std::cout << "Executing ALU operation [" << alu_ctrl[0] << alu_ctrl[1] << alu_ctrl[2] << alu_ctrl[3] << "]: ";
    }

    if(alu_ctrl[1] == 0 && alu_ctrl[2] == 1 && alu_ctrl[3] == 0) // if we are doing an add
    {
        if (debug)
        {
            std::cout << "addition" << std::endl;
        }
        if (control_signals[7] == 0) // if we are doing a R-type instruction
        {
            exe_mem_buffer->alu_result = id_exe_buffer->read_data1 + id_exe_buffer->read_data2;
        }
        else if (control_signals[7] == 1) // if we are doing an I-type instruction
        {
            exe_mem_buffer->alu_result = id_exe_buffer->read_data1 + id_exe_buffer->immediate;
        }
    }
    else if (alu_ctrl[1] == 1 && alu_ctrl[2] == 1 && alu_ctrl[3] == 0) // if we are doing a subtract
    {
        if (debug)
        {
            std::cout << "subtraction" << std::endl;
        }
        if (control_signals[7] == 0) // if we are doing a R-type instruction
        {
            exe_mem_buffer->alu_result = id_exe_buffer->read_data1 - id_exe_buffer->read_data2;
        }
        else if (control_signals[7] == 1) // if we are doing an I-type instruction
        {
            exe_mem_buffer->alu_result = id_exe_buffer->read_data1 - id_exe_buffer->immediate;
        }
    }
    else if (alu_ctrl[1] == 0 && alu_ctrl[2] == 0 && alu_ctrl[3] == 0) // if we are doing a bitwise AND
    {
        if (debug)
        {
            std::cout << "bitwise AND" << std::endl;
        }
        if (control_signals[7] == 0) // if we are doing a R-type instruction
        {
            exe_mem_buffer->alu_result = id_exe_buffer->read_data1 & id_exe_buffer->read_data2;
        }
        else if (control_signals[7] == 1) // if we are doing an I-type instruction
        {
            exe_mem_buffer->alu_result = id_exe_buffer->read_data1 & id_exe_buffer->immediate;
        }
    }
    else if (alu_ctrl[1] == 0 && alu_ctrl[2] == 0 && alu_ctrl[3] == 1) // if we are doing a bitwise OR
    {
        if (debug)
        {
            std::cout << "bitwise OR" << std::endl;
        }
        if (control_signals[7] == 0) // if we are doing a R-type instruction
        {
            exe_mem_buffer->alu_result = id_exe_buffer->read_data1 | id_exe_buffer->read_data2;
        }
        else if (control_signals[7] == 1) // if we are doing an I-type instruction
        {
            exe_mem_buffer->alu_result = id_exe_buffer->read_data1 | id_exe_buffer->immediate;
        }
    }

    // (Other ALU operations and side-effects would go here)

}

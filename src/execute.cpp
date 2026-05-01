#include <iostream>
#include "../header/execute.h"

// Implementation for the execute stage
void Execute(ID_EXE_buffer *id_exe_buffer, EXE_MEM_buffer *exe_mem_buffer, int alu_ctrl[], bool debug)
{
    if (debug)
    {
        std::cout << "EXECUTE STAGE ===============================\n";
        std::cout << "Executing ALU operation [" << alu_ctrl[0] << alu_ctrl[1] << alu_ctrl[2] << alu_ctrl[3] << "]: ";
    }

    // Buffer data handovers
    exe_mem_buffer->pc = id_exe_buffer->pc; // pass along the pc value for use in the memory stage for branch instructions
    exe_mem_buffer->rd = id_exe_buffer->rd; // pass along the destination register number for use in the memory stage and write back stage
    exe_mem_buffer->rs2_val = id_exe_buffer->read_data2; // pass along the value to store for store instructions
    exe_mem_buffer->alu_result = 0; // initialize the alu result value to 0, will be updated based on the ALU operation we perform

    // Check for each operation //

    if (ALUSrc) // if ALUSrc is 1, then the second ALU operand should be the immediate value instead of the value read from the second register
    {
        id_exe_buffer->read_data2 = id_exe_buffer->immediate; // update the second ALU operand to be the immediate value
        if (debug)
        {
            std::cout << "Using immediate value " << id_exe_buffer->immediate << " as second ALU operand" << std::endl;
        }
    }

    if (alu_ctrl[0] == 0 && alu_ctrl[1] == 0 && alu_ctrl[2] == 1 && alu_ctrl[3] == 0) // ADD (0010)
    {
        exe_mem_buffer->alu_result = id_exe_buffer->read_data1 + id_exe_buffer->read_data2;
        
        if (debug)
        {
            std::cout << "ADD " << id_exe_buffer->read_data1 << " and " << id_exe_buffer->read_data2 << " to get " << exe_mem_buffer->alu_result << std::endl;
        }
    }
    
    else if (alu_ctrl[0] == 0 && alu_ctrl[1] == 1 && alu_ctrl[2] == 0 && alu_ctrl[3] == 0) // SUB (0100)
    {
        exe_mem_buffer->alu_result = id_exe_buffer->read_data1 - id_exe_buffer->read_data2;
        if (debug)
        {
            std::cout << "SUB " << id_exe_buffer->read_data2 << " from " << id_exe_buffer->read_data1 << " to get " << exe_mem_buffer->alu_result << std::endl;
        }
        // Set the ALU zero flag for branch instructions
        if (exe_mem_buffer->alu_result == 0)
        {
            alu_zero = 1;
            if (debug)
            {
                std::cout << "ALU result is zero, setting ALU zero flag to 1" << std::endl;
            }
        }
        else
        {
            alu_zero = 0;
            if (debug)
            {
                std::cout << "ALU result is not zero, setting ALU zero flag to 0" << std::endl;
            }
        }
    }
    
    else if (alu_ctrl[0] == 0 && alu_ctrl[1] == 0 && alu_ctrl[2] == 0 && alu_ctrl[3] == 0) // AND (0000)
    {
        exe_mem_buffer->alu_result = id_exe_buffer->read_data1 & id_exe_buffer->read_data2;
        if (debug)
        {
            std::cout << "AND " << id_exe_buffer->read_data1 << " and " << id_exe_buffer->read_data2 << " to get " << exe_mem_buffer->alu_result << std::endl;
        }
    }
    
    else if (alu_ctrl[0] == 1 && alu_ctrl[1] == 0 && alu_ctrl[2] == 0 && alu_ctrl[3] == 0) // OR (1000)
    {
        exe_mem_buffer->alu_result = id_exe_buffer->read_data1 | id_exe_buffer->read_data2;
        if (debug)
        {
            std::cout << "OR " << id_exe_buffer->read_data1 << " and " << id_exe_buffer->read_data2 << " to get " << exe_mem_buffer->alu_result << std::endl;
        }
    }

    else if (alu_ctrl[0] == 1 && alu_ctrl[1] == 1 && alu_ctrl[2] == 0 && alu_ctrl[3] == 0) // XOR (1100)
    {
        exe_mem_buffer->alu_result = id_exe_buffer->read_data1 ^ id_exe_buffer->read_data2;
        if (debug)
        {
            std::cout << "XOR " << id_exe_buffer->read_data1 << " and " << id_exe_buffer->read_data2 << " to get " << exe_mem_buffer->alu_result << std::endl;
        }
    }
    // Calculate the branch or Jump target address for branch instructions
    if (Branch) // if this is a branch instruction, we need to calculate the branch target address for use in the fetch stage
    {
        branch_target = id_exe_buffer->pc + id_exe_buffer->immediate; // the branch target address is the current pc value plus the sign-extended immediate value
        if (debug)
        {
            std::cout << "Calculating branch target address: " << id_exe_buffer->pc << " + " << id_exe_buffer->immediate << " = " << branch_target << std::endl;
        }
    }
    if (Jump)
    {
        branch_target = id_exe_buffer->immediate;
    }
    
    if (debug)
    {
        std::cout << "============================================\n" << std::endl;
    }
}

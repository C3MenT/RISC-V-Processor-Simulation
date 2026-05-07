#include <iostream>
#include "../header/execute.h"

// Implementation for the execute stage
void Execute(ID_EXE_buffer *id_exe_buffer, EXE_MEM_buffer *exe_mem_buffer, int alu_ctrl[], bool debug)
{
    if (STALL)
    {
        // Check if we must stall still
        STALL--; // remove 1 for MEM stage and then 1 for WB

        if (debug)
            printf("STALLING...\n");
        return;
    }

    if (FLUSH == 3) // if we must flush up to EXE
    {
        id_exe_buffer->rd = 0;
        id_exe_buffer->read_data1 = 0;
        id_exe_buffer->read_data2 = 0;
        id_exe_buffer->pc_target = 0;
        id_exe_buffer->RegWrite = 0;
        id_exe_buffer->MemWrite = 0;
        id_exe_buffer->MemtoReg = 0;
        id_exe_buffer->MemRead = 0;
        id_exe_buffer->Branch = 0;
        id_exe_buffer->Jump = 0;
        id_exe_buffer->PCSrc = 0;
        FLUSH--;
    }

    if (debug)
    {
        if (pipeline)
        {
            static int inst_index = -2;
            inst_index++;
            printf("\nEXECUTE STAGE (%d) ===============================\n", inst_index);
        }
        else
            printf("\nEXECUTE STAGE ===============================\n");
        std::cout << "Executing ALU operation [" << id_exe_buffer->ALU_CTRL[0] << id_exe_buffer->ALU_CTRL[1] << id_exe_buffer->ALU_CTRL[2] << id_exe_buffer->ALU_CTRL[3] << "]: ";
    }

    if (pipeline)
    {
        // Hazard Detection / Forwarding //
        
        // // (Need last ALU instr's rd) ==============================================
        // We must check if our rs1 or rs2 if existing are the rd of the previous instr
        // If so we must forward whatever the previous result was 
        if (id_exe_buffer->rs1 == exe_mem_buffer->rd && !exe_mem_buffer->MemtoReg)
        {
            id_exe_buffer->read_data1 = exe_mem_buffer->alu_result;
        }
        if (id_exe_buffer->rs2 == exe_mem_buffer->rd && !exe_mem_buffer->MemtoReg)
        {
            id_exe_buffer->read_data2 = exe_mem_buffer->alu_result;
        }
        
        // // (An instr in mem or wb still and is a load)
        if ((id_exe_buffer->rs1 == exe_mem_buffer->rd || id_exe_buffer->rs2 == exe_mem_buffer->rd) && exe_mem_buffer->MemtoReg)
        {
            STALL = 2;
        }
    }

    // Buffer data handovers
    exe_mem_buffer->pc = id_exe_buffer->pc; // pass along the pc value for use in the memory stage for branch instructions
    exe_mem_buffer->rd = id_exe_buffer->rd; // pass along the destination register number for use in the memory stage and write back stage
    exe_mem_buffer->rs2_val = id_exe_buffer->read_data2; // pass along the value to store for store instructions
    exe_mem_buffer->alu_result = 0; // initialize the alu result value to 0, will be updated based on the ALU operation we perform
    exe_mem_buffer->pc_target = id_exe_buffer->pc_target;

    exe_mem_buffer->RegWrite = id_exe_buffer->RegWrite; // pass along the control signals to the exe/mem buffer for use in the memory stage and write back stage
    exe_mem_buffer->MemWrite = id_exe_buffer->MemWrite;
    exe_mem_buffer->MemtoReg = id_exe_buffer->MemtoReg;
    exe_mem_buffer->MemRead = id_exe_buffer->MemRead;
    exe_mem_buffer->Branch = id_exe_buffer->Branch;
    exe_mem_buffer->Jump = id_exe_buffer->Jump;
    exe_mem_buffer->PCSrc = id_exe_buffer->PCSrc;
    exe_mem_buffer->ALU_Zero = 0; // initialize ALU Zero as 0

    // Check for each operation //

    if (id_exe_buffer->ALUSrc) // if ALUSrc is 1, then the second ALU operand should be the immediate value instead of the value read from the second register
    {
        id_exe_buffer->read_data2 = id_exe_buffer->immediate; // update the second ALU operand to be the immediate value
        if (debug)
        {
            std::cout << "Using immediate value " << id_exe_buffer->immediate << " as second ALU operand" << std::endl;
        }
    }

    if (!id_exe_buffer->ALU_CTRL[0] && !id_exe_buffer->ALU_CTRL[1] && id_exe_buffer->ALU_CTRL[2] && !id_exe_buffer->ALU_CTRL[3]) // ADD (0010)
    {
        exe_mem_buffer->alu_result = id_exe_buffer->read_data1 + id_exe_buffer->read_data2;
        
        if (debug)
        {
            std::cout << "ADD " << id_exe_buffer->read_data1 << " and " << id_exe_buffer->read_data2 << " to get " << exe_mem_buffer->alu_result << std::endl;
        }
        
        if (id_exe_buffer->Jump)
        jalr_target = exe_mem_buffer->alu_result; // forward rs1 + immediate

        if (pipeline) // in pipelined implementation we check for a jalr instr to forward target
        {
            if (debug)
                printf("JUMP %d and PCSrc==3(%d) = %d\n", id_exe_buffer->Jump, id_exe_buffer->PCSrc, id_exe_buffer->Jump && id_exe_buffer->PCSrc == 3);
            if (id_exe_buffer->Jump && id_exe_buffer->PCSrc == 3) // if a Jalr
            {
                //pc = exe_mem_buffer->alu_result; // pc = rs1 + immediate (found in ALU just now)
                
                // Now that we have the jalr_target, this is the soonest we can assert the jump signals
                Jump = 1;
                PCSrc = 3;
                FLUSH = 2; // "flush" the next iteration's IF and flush the next ID stage as they will be outdated
                if (debug)
                    printf("Set to jump to %d\n", jalr_target);
            }
        }
    }
    
    else if (!id_exe_buffer->ALU_CTRL[0] && id_exe_buffer->ALU_CTRL[1] && id_exe_buffer->ALU_CTRL[2] && !id_exe_buffer->ALU_CTRL[3]) // SUB (0110)
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
            exe_mem_buffer->ALU_Zero = 1;
            if (id_exe_buffer->Branch)
                PCSrc = 1;
            if (debug)
            {
                std::cout << "ALU result is zero, setting ALU zero flag to 1" << std::endl;
            }
        }
        else
        {
            alu_zero = 0;
            //exe_mem_buffer->ALU_Zero = 0; // initialized as 0
            if (debug)
            {
                std::cout << "ALU result is not zero, setting ALU zero flag to 0" << std::endl;
            }
        }
        if (pipeline) // We check whether to forward the branch outcome if we have a pipeline
        {
            if (id_exe_buffer->Branch && exe_mem_buffer->ALU_Zero)
            {
                //pc = id_exe_buffer->pc_target; // pc = pc + immediate (found by Decode "Adder")
                FLUSH = 2; // set to flush previous two stages
                //if (debug)
                //    std::cout << "Forwarded Branch taken, updating program counter to branch target address " << id_exe_buffer->pc_target << std::endl;
                //printf("pc is modified to 0x%x\n", pc);
            }
        }
    }
    
    else if (!id_exe_buffer->ALU_CTRL[0] && !id_exe_buffer->ALU_CTRL[1] && !id_exe_buffer->ALU_CTRL[2] && !id_exe_buffer->ALU_CTRL[3]) // AND (0000)
    {
        exe_mem_buffer->alu_result = id_exe_buffer->read_data1 & id_exe_buffer->read_data2;
        if (debug)
        {
            std::cout << "AND " << id_exe_buffer->read_data1 << " and " << id_exe_buffer->read_data2 << " to get " << exe_mem_buffer->alu_result << std::endl;
        }
    }
    
    else if (!id_exe_buffer->ALU_CTRL[0] && !id_exe_buffer->ALU_CTRL[1] && !id_exe_buffer->ALU_CTRL[2] && id_exe_buffer->ALU_CTRL[3]) // OR (0001)
    {
        exe_mem_buffer->alu_result = id_exe_buffer->read_data1 | id_exe_buffer->read_data2;
        if (debug)
        {
            std::cout << "OR " << id_exe_buffer->read_data1 << " and " << id_exe_buffer->read_data2 << " to get " << exe_mem_buffer->alu_result << std::endl;
        }
    }

    else if (id_exe_buffer->ALU_CTRL[0] && id_exe_buffer->ALU_CTRL[1] && !id_exe_buffer->ALU_CTRL[2] && !id_exe_buffer->ALU_CTRL[3]) // XOR (1100)
    {
        exe_mem_buffer->alu_result = id_exe_buffer->read_data1 ^ id_exe_buffer->read_data2;
        if (debug)
        {
            std::cout << "XOR " << id_exe_buffer->read_data1 << " and " << id_exe_buffer->read_data2 << " to get " << exe_mem_buffer->alu_result << std::endl;
        }
    }
    if (debug)
    {
        exe_mem_buffer->print_buffer();
        std::cout << "============================================\n" << std::endl;
    }
}

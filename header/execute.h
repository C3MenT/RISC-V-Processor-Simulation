#pragma once
#include "datapath.h"

/// @brief Execute Stage function performs the ALU operations for the instruction based on the control 
/// signals set by the decode stage and stores the result in the EXE/MEM buffer for use in the memory stage.
/// Formally this: 1 Does ALU operation given by alu_ctrl, 2 updates alu_zero flag, and 3 calculates branch_target if applicable.
/// @param id_exe_buffer 
/// @param exe_mem_buffer 
/// @param alu_ctrl A four-bit control signal that indicates which ALU operation to perform. Implemented as a int array.
/// @param debug // debug flag for debug output
void execute(ID_EXE_buffer *id_exe_buffer, EXE_MEM_buffer *exe_mem_buffer, int alu_ctrl[], bool debug = false);
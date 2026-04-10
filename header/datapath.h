#pragma once
/*
    This header contains all the shared objects templates and globals that are used
    my multiple different cpp files in the program.
*/

// Program Counter, global variable to keep track of instruction address. 
// Incremented by 4 after each instruction as instructions are 4 bytes long. 
// Initialized to 0 as the first instruction is at address 0.
int extern pc = 0;
// the current pc value + 4
int extern next_pc = 4; 

// // Buffers ====================================================================
typedef struct IF_ID_buffer
{
    const char* instruction; // 32-bit machine code instruction as a string of 1s and 0s
    int pc; // potentially used pc + 4 value
} IF_ID_buffer;

//IF_ID_buffer if_id_buffer; // input buffer for the decode stage

typedef struct ID_EXE_buffer
{
    //const char* instruction; // actual name of the instruction as a string (e.g. "add", "lw", etc.)
    int pc; // potentially used pc + 4 value
    // register values and immediate value for use in the execute stage =====================
    int read_data1; // value read from rs1
    int read_data2; // value read from rs2 (for R-type) or the immediate value (for I-type)
    int immediate; // the sign-extended immediate value for I-type, S-type, SB-type, U-type, and UJ-type instructions
    // Register indices =====================================================================
    int rs1; // source register 1 number (0-31)
    int rs2; // source register 2 number (0-31) for R-type or 0 for I-type
    int rd; // destination register number (0-31)
    // Control signals for use in the control stage ==========================================
    int control_signals[7]; // control signals for the instruction, stored as an array of integers representing binary values (0 or 1)
} ID_EXE_buffer;

//ID_EXE_buffer id_exe_buffer; // output buffer for the decode stage and input for the execute stage

typedef struct EXE_MEM_buffer
{
    int pc; // correct pc value to use in the memory stage for branch instructions
    int alu_result;
    int rs2_val; // for store instructions
    int rd; // destination register number (0-31)
} EXE_MEM_buffer;

//EXE_MEM_buffer exe_mem_buffer; // output buffer for the execute stage and input for the memory stage

typedef struct MEM_WB_buffer
{
    int mem_result; // for load instructions
    int alu_result; // for R-type and I-type instructions
    int rd; // destination register number (0-31)
} MEM_WB_buffer;

// Register file ==================================================================
// 32 registers each 32 bits wide, initialized to 0
// Note that register 0 is hardwired to 0 and cannot be modified, so it will always contain the value 0.
// We use one array so index is the reg name and value is kept as decimal int
int rf[32] = {0};
/* register guide: 
x0: zero, 
x1: ra, 
x2: sp, 
x3: gp, 
x4: tp, 
x5-x7: t0-t2, 
x8-x9: s0-s1, 
x10-x17: a0-a7, 
x18-x27: s2-s11, 
x28-x31: t3-t6
*/

// Control Unit ==================================================================
/*
bool RegWrite = false; // whether to write back to the register file
bool Branch = false; // whether the instruction is a branch instruction, used to determine whether to update the program counter with the branch target address
bool ALUSrc = false; // whether to use the immediate value instead of the second register value as the second ALU operand
bool MemWrite = false; // whether to write to memory
bool MemtoReg = false; // whether to write back the memory result instead of the ALU result to the register file
bool MemRead = false; // whether to read from memory
*/
// control signals in order: 0 RegWrite, 1 Branch, 2 ALUSrc, 3 MemWrite, 4 MemtoReg, 5 MemRead 6 ALUOp (2 bits)
// values are in decimal but represent binary values, so 0 is false and 1 is true for all except ALUOp 
// which is 0 for R-type, 1 for I-type, and 2 for all other types
int control_signals[7] = {0};


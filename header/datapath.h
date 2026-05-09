#pragma once
#include <iostream>
/*
    This header contains all the shared objects templates and globals that are used
    my multiple different cpp files in the program.
*/


// Run Flags ==================================================================
extern bool pipeline; // whether to run the simulation in pipelined mode or sequential mode, default is sequential
extern bool use_reg_names; // whether or not to use register names instead of indices.
// ============================================================================

// Program Counter, global variable to keep track of instruction address. 
// Incremented by 4 after each instruction as instructions are 4 bytes long. 
// Initialized to 0 as the first instruction is at address 0.
extern int pc;

// calculate default next pc value for the next instruction to read in the next cycle
extern int next_pc;

// Register file ==================================================================
// 32 registers each 32 bits wide, initialized to 0
// Note that register 0 is hardwired to 0 and cannot be modified, so it will always contain the value 0.
// We use one array so index is the reg name and value is kept as decimal int
extern int rf[32];
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

extern int RegWrite; // whether to write back to the register file
extern int Branch; // whether the instruction is a branch instruction, used to determine whether to update the program counter with the branch target address
extern int ALUSrc; // whether to use the immediate value instead of the second register value as the second ALU operand
extern int ALUOp[2]; // the ALU operation to perform
extern int MemWrite; // whether to write to memory
extern int MemtoReg; // whether to write back the memory result instead of the ALU result to the register file
extern int MemRead; // whether to read from memory
extern int Jump; // whether the instruction is a jump instruction, used to determine whether to update the program counter with the jump target address

// control signals in order: 0 RegWrite, 1 Branch, 2 ALUSrc, 3 MemWrite, 4 MemtoReg, 5 MemRead, 6 ALUOp (2 bits), 7 Jump
// values are in decimal but represent binary values, so 0 is false and 1 is true for all except ALUOp 
// which is 0 for R-type, 1 for I-type, and 2 for all other types
//extern int control_signals[8];


extern int alu_ctrl[4]; // For exe stage

extern int alu_ctrl[4]; // For exe stage

// whether the result of the ALU operation is zero, used for branch instructions;
// Guidelines say this must be a global var named "alu-zero" but we can't use a hyphen
extern int alu_zero;

// Control that determines whether next PC comes from PC+4, jal target, jalr target, or branch target 
// This is set through multiple muxes physically, but here we will use enumeration
// [0 = PC+4, 1 = Branch (PC + imm {if condition}), 2 = Jal (pc + imm), 3 = Jalr (rs1 + imm)]
extern int PCSrc; 

// the target address to update the program counter to if we are taking a branch
// Guidelines say this must be a global var initialized to 0, used by Fetch() and Execute() functions
extern int branch_target;

// akin to branch target, but for jal instructions
extern int jal_target;

// akin to branch target, but for jalr instructions
extern int jalr_target;

// Emitable flush signal for hazard detection
// Should be equal to the amount of stages that should be flushed (0, 1, or 2)
extern int FLUSH;

// we keep a stall flag that counts how many stages from the beginning to stall
// 0 = none, 1 = IF, 2 = ID, 3 = EXE
extern int STALL;

// // Buffers ====================================================================
typedef struct IF_ID_buffer
{
    char instruction[33]; // 32-bit machine code instruction as a string of 1s and 0s (32 characters + null terminator)
    int pc; // potentially used pc + 4 value
    int instr_index; // an enumeration for each instruction
    
    IF_ID_buffer() // constructor to initialize the buffer values to 0
    {
        for (int i = 0; i < 33; i++)
        {
            instruction[i] = '0'; // initialize instruction to all 0s
        }
        pc = 0; // initialize pc value in IF/ID buffer to 0
        instr_index = 0;
    }
    void print_buffer()
    {
        std::cout << "\nIF/ID Buffer Values:\n";
        std::cout << "PC: " << pc << "\n";
        std::cout << "Instruction: " << instruction << "\n";
    }
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
    int rs1; // source register 1 number (0-31) (Possibly Unnecessary)
    int rs2; // source register 2 number (0-31) for R-type or 0 for I-type (Possibly Unnecessary)
    int rd; // destination register number (0-31)
    int pc_target; // target address to jump to in case we have a Jal (only jal can set this as early as decode stage)
    // Control Signals =====================================================================
    // ID/EXE has all but subsequent buffers will logically have less and less of these as they get used up
    int RegWrite; // whether to write back to the register file
    int Branch; // whether the instruction is a branch instruction, used to determine whether to update the program counter with the branch target address
    int ALUSrc; // whether to use the immediate value instead of the second register value as the second ALU operand
    int MemWrite; // whether to write to memory
    int MemtoReg; // whether to write back the memory result instead of the ALU result to the register file
    int MemRead; // whether to read from memory
    int Jump; // whether the instruction is a jump instruction, used to determine whether to update the program counter with the jump target address
    int ALU_CTRL[4]; // the actual ALU control signals to determine which ALU operation to perform in the execute stage
    // Control that determines whether next PC comes from PC+4, jal target, jalr target, or branch target 
    // This is set through multiple muxes physically, but here we will use enumeration
    // [0 = PC+4, 1 = Branch (PC + imm {if condition}), 2 = Jal (pc + imm), 3 = Jalr (rs1 + imm)]
    int PCSrc;
    int instr_index; // an enumeration for each instruction
    
    // Constructor to initialize all values to 0
    ID_EXE_buffer()
    {
        pc = 0;
        read_data1 = 0;
        read_data2 = 0;
        immediate = 0;
        rs1 = 0;
        rs2 = 0;
        rd = 0;
        pc_target = 0;

        RegWrite = 0;
        Branch = 0;
        ALUSrc = 0;
        MemWrite = 0;
        MemtoReg = 0;
        MemRead = 0;
        Jump = 0;
        ALU_CTRL[0] = 0; ALU_CTRL[1] = 0; ALU_CTRL[2] = 0; ALU_CTRL[3] = 0;
        PCSrc = 0;
        instr_index = 0;
    }
    void nop()
    {
        pc = 0;
        read_data1 = 0;
        read_data2 = 0;
        immediate = 0;
        rs1 = 0;
        rs2 = 0;
        rd = 0;
        pc_target = 0;

        RegWrite = 0;
        Branch = 0;
        ALUSrc = 0;
        MemWrite = 0;
        MemtoReg = 0;
        MemRead = 0;
        Jump = 0;
        ALU_CTRL[0] = 0; ALU_CTRL[1] = 0; ALU_CTRL[2] = 0; ALU_CTRL[3] = 0;
        PCSrc = 0;
        instr_index = 0;
    }
    void print_buffer()
    {
        std::cout << "\nID/EXE Buffer Values:\n";
        std::cout << "PC: " << pc << "\n";
        std::cout << "Read Data 1: " << read_data1 << " ";
        std::cout << "Read Data 2: " << read_data2 << " ";
        std::cout << "Immediate: " << immediate << "\n";
        std::cout << "RS1: " << rs1 << " ";
        std::cout << "RS2: " << rs2 << " ";
        std::cout << "RD: " << rd << "\n";
        std::cout << "PC Target: " << pc_target << "\n";
        std::cout << "Control Signals:\n";
        std::cout << "RegWrite: " << RegWrite << " ";
        std::cout << "Branch: " << Branch << " ";
        std::cout << "ALUSrc: " << ALUSrc << " ";
        std::cout << "MemWrite: " << MemWrite << " ";
        std::cout << "MemtoReg: " << MemtoReg << " ";
        std::cout << "MemRead: " << MemRead << " ";
        std::cout << "Jump: " << Jump << " ";
        std::cout << "ALU Control Signals: [" << ALU_CTRL[0] << ALU_CTRL[1] << ALU_CTRL[2] << ALU_CTRL[3] << "]\n";
        std::cout << "PCSrc: " << PCSrc << "\n";
    }

} ID_EXE_buffer;

//ID_EXE_buffer id_exe_buffer; // output buffer for the decode stage and input for the execute stage

typedef struct EXE_MEM_buffer
{
    int pc; // "Next" pc + 4 value to use by default to get next instruction
    int alu_result; // the ALU result or address offset from EXE stage
    int rs1_val; // r1 value for store and load addresses
    int rs2_val; // r2 value to store for store instructions
    int rd; // destination register number (0-31)
    int pc_target; // If we have a branch or jump target this stores it
    // Control signals for memory stage
    int RegWrite; // whether to write back to the register file
    int Branch; // whether the instruction is a branch instruction, used to determine whether to update
    int Jump; // whether the instruction is a jump instruction, used to determine whether to update the program counter with the jump target address
    int MemWrite; // whether to write to memory
    int MemtoReg; // whether to write back the memory result instead of the ALU result to the register file
    int MemRead; // whether to read from memory
    // Control that determines whether next PC comes from PC+4, jal target, jalr target, or branch target 
    // This is set through multiple muxes physically, but here we will use enumeration
    // [0 = PC+4, 1 = Branch (PC + imm {if condition}), 2 = Jal (pc + imm), 3 = Jalr (rs1 + imm)]
    int PCSrc; 
    int ALU_Zero; // whether an ALU Sub operation resulted in 0
    int instr_index; // an enumeration for each instruction

    EXE_MEM_buffer() // constructor to initialize all values to 0
    {
        pc = 0;
        alu_result = 0;
        rs1_val = 0;
        rs2_val = 0;
        rd = 0;
        pc_target = 0;

        RegWrite = 0;
        Branch = 0;
        Jump = 0;
        MemWrite = 0;
        MemtoReg = 0;
        MemRead = 0;
        PCSrc = 0;
        ALU_Zero = 0;
        instr_index = 0;
    }
    void nop()
    {
        pc = 0;
        alu_result = 0;
        rs1_val = 0;
        rs2_val = 0;
        rd = 0;
        pc_target = 0;

        RegWrite = 0;
        Branch = 0;
        Jump = 0;
        MemWrite = 0;
        MemtoReg = 0;
        MemRead = 0;
        PCSrc = 0;
        ALU_Zero = 0;
        instr_index = 0;
    }
    void print_buffer()
    {
        std::cout << "\nEXE/MEM Buffer Values:\n";
        std::cout << "PC: " << pc << "\n";
        std::cout << "ALU Result: " << alu_result << "\n";
        std::cout << "RS1 Value: " << rs1_val << " ";
        std::cout << "RS2 Value: " << rs2_val << " ";
        std::cout << "RD: " << rd << "\n";
        std::cout << "PC Target: " << pc_target << "\n";
        std::cout << "Control Signals:\n";
        std::cout << "RegWrite: " << RegWrite << " ";
        std::cout << "Branch: " << Branch << " ";
        std::cout << "Jump: " << Jump << " ";
        std::cout << "MemWrite: " << MemWrite << " ";
        std::cout << "MemtoReg: " << MemtoReg << " ";
        std::cout << "MemRead: " << MemRead << "\n";
        std::cout << "PCSrc : " << PCSrc << "\n"; 
        std::cout << "ALU Zero : " << ALU_Zero << "\n";
    }
} EXE_MEM_buffer;

//EXE_MEM_buffer exe_mem_buffer; // output buffer for the execute stage and input for the memory stage

typedef struct MEM_WB_buffer
{
    int pc;
    int mem_result; // for load instructions
    int alu_result; // for R-type and I-type instructions
    int rd; // destination register number (0-31)
    int pc_target; // If we have a branch or jump target this stores it
    // Control signals for write back stage
    int RegWrite; // whether to write back to the register file
    int MemtoReg; // whether to write back the memory result instead of the ALU result to the register file
    int Branch; // whether the instruction is a branch instruction, used to determine whether to update the program counter with the branch target address
    int Jump; // whether the instruction is a jump instruction, used to determine whether to update the program counter with the jump target address
    // Control that determines whether next PC comes from PC+4, jal target, jalr target, or branch target 
    // This is set through multiple muxes physically, but here we will use enumeration
    // [0 = PC+4, 1 = Branch (PC + imm {if condition}), 2 = Jal (pc + imm), 3 = Jalr (rs1 + imm)]
    int PCSrc;
    int ALU_Zero; // whether an ALU Sub operation resulted in 0
    int instr_index; // an enumeration for each instruction

    MEM_WB_buffer() // constructor to initialize all values to 0
    {
        pc = 0;
        mem_result = 0;
        alu_result = 0;
        rd = 0;
        pc_target = 0;

        RegWrite = 0;
        MemtoReg = 0;
        Branch = 0;
        Jump = 0;
        PCSrc = 0;
        ALU_Zero = 0;
        instr_index = 0;
    }
    void print_buffer()
    {
        std::cout << "\nMEM/WB Buffer Values:\n";
        std::cout << "PC: " << pc << "\n";
        std::cout << "Memory Result: " << mem_result << " ";
        std::cout << "ALU Result: " << alu_result << " ";
        std::cout << "RD: " << rd << "\n";
        std::cout << "PC Target: " << pc_target << "\n";
        std::cout << "Control Signals:\n";
        std::cout << "RegWrite: " << RegWrite << " ";
        std::cout << "Branch: " << Branch << " ";
        std::cout << "Jump: " << Jump << " ";
        std::cout << "MemtoReg: " << MemtoReg << "\n";
        std::cout << "PCSrc : " << PCSrc << "\n";
        std::cout << "ALU Zero : " << ALU_Zero << "\n";
    }
} MEM_WB_buffer;

extern int d_mem[32];

extern int total_clock_cycles;


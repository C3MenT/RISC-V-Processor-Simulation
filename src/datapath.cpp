/* Datapath variables 
    aka the physical declarations of the globals
*/
#include "../header/datapath.h"

bool pipeline = true; // whether to run the simulation in pipelined mode or sequential mode, default is sequential
bool use_reg_names = true; // whether or not to use register names instead of indices.

int pc = 0; // initialize program counter to 0
int rf[32] = {0}; // initialize register file to all 0s

// Instructions demand a seperate global variable for each signal
int RegWrite = 0; // whether to write back to the register file
int Branch = 0; // whether the instruction is a branch instruction, used to determine whether to update the program counter with the branch target address
int ALUSrc = 0; // whether to use the immediate value instead of the second register value as the second ALU operand
int ALUOp[2] = {0}; // the ALU operation to perform
int MemWrite = 0; // whether to write./ to memory
int MemtoReg = 0; // whether to write back the memory result instead of the ALU result to the register file
int MemRead = 0; // whether to read from memory
int Jump = 0; // whether the instruction is a jump instruction, used to determine whether to update the program counter with the jump target address
//int control_signals[8] = {0}; // initialize control signals to all 0s

int alu_ctrl[4] = {0}; // For exe stage
int alu_zero = 0; // initialize ALU zero flag to 0
int d_mem[32]= {0}; //initialize data memory to all 0s
int total_clock_cycles = 0; //initialize total clock cycles to 0
int branch_target = 0; // initialize branch target to 0

int FLUSH = 0;
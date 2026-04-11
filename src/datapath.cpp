/* Datapath variables 
    aka the physical declarations of the globals
*/
#include "../header/datapath.h"

int pc = 0; // initialize program counter to 0
int rf[32] = {0}; // initialize register file to all 0s
int control_signals[8] = {0}; // initialize control signals to all 0s
int alu_zero = 0; // initialize ALU zero flag to 0
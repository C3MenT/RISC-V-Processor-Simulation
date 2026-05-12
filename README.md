CSE-140 RISC-V Datapath Simulation Project
by Ri'chard McCray and Sivani Potta

##### Usage Overview #####

Build:
The program can be built using the provided Makefile and GNU Make 4.3.
Run the "make" command in the terminal from the "RISC-V Processor Simulation" directory.
The source and header files must be in the provided directories. relative to the Makefile.

Run Single-Cycle Mode:
The "risc_simulator.exe" executable can be ran from the terminal with no arguments to process a program with single-cycle execution by default. If no arguments are provided, the program automatically uses the single-cycle implementation loop and prompts the user for the relative directory of an input text file containing machine code. The program will continue as normal once one is provided.

For example in wsl terminal: "./risc_processor.exe"
Then enter: "tests/sample_part1.txt" (or some other file)

Alternatively, the program can have the input file path entered as an argument in the terminal. The program will not prompt the user for input if one is provided in the terminal beforehand.

For example: "./risc_processor.exe {input file path}"

Run Pipelined Mode:
The program can be run in pipelined implementation from the terminal by adding a "-p" flag after the input file path. 
(The program can technically be run in pipelined implementation by default by changing the "pipeline" global variable to true in datapath.cpp and recompiling before running the executable with no arguments as well).

For example: "./risc_processor.exe {input file path} -p"

Debug Output:
For extensive debug output, an additional "-d" flag can be included as the 2nd or third argument after the input file path in the terminal.

For example: "./risc_processor.exe {input file path} -p -d" or "./risc_processor.exe {input file path} -d -p"


Input Format:
The input file is expected to be a scannable text file with one 32-bit RISC-V machine instruction per line.

Program Output:
The program outputs each ending cycle per iteration as well as what values may have been changed during execution.

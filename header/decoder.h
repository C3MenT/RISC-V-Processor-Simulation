#pragma once
#include "datapath.h"

// Global "Dictionary" of opcode values to their type
//extern const char* opcodes[13][2];

/// @brief Main decoding function determines the instruction type of a passed in machine
/// code instruction and relevant fields to print out.

/// @param debug boolean flag to enable debug output
void Decode(int *reg_file, IF_ID_buffer *if_id_buffer, ID_EXE_buffer *id_exe_buffer, bool debug = false);

/// @brief Populates the control signals based on given instruction type. 
/// This is used by the ALU Control to determine which ALU operation to perform in the execute stage.
/// @param type_name Type name as a string (R, I, S, SB, U, UJ) recieved from get_type() function
/// @param opcode the opcode of the instruction as a string (7-digit binary) recieved from get_opcode() function for special distinctions
void Control_Unit(const char* type_name, const char* opcode);

/// @brief Returns the decimal representation of a binary number as an integer
/// @param bin the binary number to convert as an integer
/// @return the binary number's representation in decimal as an integer
int decimal(const char* bin);

/// @brief Function retrieves the respective integer 0-9 from the ascii char
/// @param a the character of the digit to convert
/// @return the digit as an integer
int int_from_ascii(char a);

/// @brief Compares two const char strings char by char
/// @param a 
/// @param b 
/// @return 
int string_comp(const char* a, const char* b);

/// @brief Function returns the opcode specifically from a given instruction as a 7-digit integer value
/// @param instruction 32-bit machine code instruction
/// @return the last 7 bits of the passed instruction as an integer
const char* get_opcode(const char* instruction);

/// @brief Function returns an string of the instruction type that the given opcode maps to
/// @param opcode the 7 digit opcode from a machine instruction
/// @return the integer enumertaion of the respective type for the instruction
const char* get_type(const char* opcode);

/// @brief Funtion gets the instruction name given an opcode, funct3, and funct7 value as strings
/// @param opcode 
/// @param funct3 
/// @param funct7 
/// @return the name of the respective instruction as a string
const char* get_name(const char* opcode, const char* funct3, const char* funct7);


char* gf_rs1(const char* instruction);
char* gf_rs2(const char* instruction);
char* gf_rd(const char* instruction);
char* gf_funct3(const char* instruction);
char* gf_funct7(const char* instruction);
char* gf_imm(const char* instruction, int start, int end);

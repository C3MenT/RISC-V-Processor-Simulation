#include <stdio.h>
#include <string.h>
#include "../header/decoder.h"

struct temp_str
{
    char* str;
    int size;
};

void Decode(int *reg_file, IF_ID_buffer *if_id_buffer, ID_EXE_buffer *id_exe_buffer, int *control_signals, bool debug)
{
    // Extract the instruction from the IF/ID buffer
    const char* instruction = if_id_buffer->instruction;

    // First we get the opcode
    const char* opcode = get_opcode(instruction);

    // Possibly relevant fields
    char* rs1 = new char[1]; *rs1 = '\0'; 
    char* rs2 = new char[1]; *rs2 = '\0';
    char* rd = new char[1]; *rd = '\0';
    char* funct3 = new char[1]; *funct3 = '\0';
    char* funct7 = new char[1]; *funct7 = '\0';
    // below must stay modifiable to any size
    // so we dynamically delcare them later and clean up;
    char* imm = new char[1]; *imm = '\0';
    char* imm1 = new char[1]; *imm1 = '\0';
    char* imm2 = new char[1]; *imm2 = '\0';
    char* imm3 = new char[1]; *imm3 = '\0';
    char* imm4 = new char[1]; *imm4 = '\0';

    // String name of the instruction type
    const char* type_name = get_type(opcode);
    // String name of the instruction. We need all other fields first
    const char* name;

    if (type_name  == "R")
    {
        delete[] rd; rd = gf_rd(instruction);
        delete[] funct3; funct3 = gf_funct3(instruction);
        delete[] rs1; rs1 = gf_rs1(instruction);
        delete[] rs2; rs2 = gf_rs2(instruction);
        delete[] funct7; funct7 = gf_funct7(instruction);
    }
    else if (type_name == "I")
    {
        delete[] rd; rd = gf_rd(instruction);
        delete[] funct3; funct3 = gf_funct3(instruction);
        delete[] rs1; rs1 = gf_rs1(instruction);
        delete[] imm; imm = gf_imm(instruction, 31, 20); // imm [11:0]
    }
    else if (type_name == "S")
    {
        delete[] imm1; imm1 = gf_imm(instruction, 11, 7); // imm [4:0]
        delete[] imm2; imm2 = gf_imm(instruction, 31, 25); // imm [11:5]
        // concatenate the immediate value
        delete[] imm; imm = new char[13];
        *imm = '\0'; // clear immm
        imm = strcat(imm, imm2); // slide in left part
        imm = strcat(imm, imm1); // slide in right part
        imm[12] = '\0'; // end with terminator

        delete[] funct3; funct3 = gf_funct3(instruction);
        delete[] rs1; rs1 = gf_rs1(instruction);
        delete[] rs2; rs2 = gf_rs2(instruction);
    }
    else if (type_name == "SB")
    {
        delete[] imm1; imm1 = gf_imm(instruction, 11, 8); // imm [4:1]
        delete[] imm2; imm2 = gf_imm(instruction, 30, 25); // imm [10:5]
        delete[] imm3; imm3 = gf_imm(instruction, 7, 7); // imm [11]
        delete[] imm4; imm4 = gf_imm(instruction, 31, 31); // imm [12]
        // reconstruct immediate by add parts with their correct 10s place power
        //imm = (imm4) + (imm3) + (imm2) + (imm1);
        delete[] imm1; imm = new char[14];
        *imm = '\0'; // clear immm
        imm = strcat(imm, imm4);
        imm = strcat(imm, imm3);
        imm = strcat(imm, imm2);
        imm = strcat(imm, imm1);
        imm = strcat(imm, "0"); // we prepend a 0 for alignment
        imm[13] = '\0';

        delete[] funct3; funct3 = gf_funct3(instruction);
        delete[] rs1; rs1 = gf_rs1(instruction);
        delete[] rs2; rs2 = gf_rs2(instruction);
    }
    else if (type_name == "U")
    {
        delete[] imm; imm = gf_imm(instruction, 31, 12); // imm [31:12]
        delete[] rd; rd = gf_rd(instruction);
    }
    else if (type_name == "UJ") // UJ type
    {
        delete[] imm1; imm1 = gf_imm(instruction, 19, 12); // imm[19:12]
        delete[] imm2; imm2 = gf_imm(instruction, 20, 20); // imm[11]
        delete[] imm3; imm3 = gf_imm(instruction, 30, 21); // imm[10:1]
        delete[] imm4; imm4 = gf_imm(instruction, 31, 31); // imm[20]
        //imm = (imm4 + imm1 + imm2 + imm3)
        delete[] imm; imm = new char[22]; // dynamically declare 20 bit immediate str
        *imm = '\0'; // clear immm
        imm = strcat(imm, imm4);
        imm = strcat(imm, imm1);
        imm = strcat(imm, imm2);
        imm = strcat(imm, imm3);
        imm = strcat(imm, "0"); // we prepend a 0 for alignment
        imm[21] = '\0';

        delete[] rd; rd = gf_rd(instruction);
    }
    else
    {
        type_name = "NOT FOUND";
    }

    // Get name of instruction
    name = get_name(opcode, funct3, funct7);

    if (debug){
        // Print Sequence (Now for debug purposes)
        printf("\nInstruction Type: %s\n", type_name);
        printf("Operation: %s\n", name);
        if (*rs1)
        {printf("Rs1: x%d\n", decimal(rs1));}
        if (*rs2)
        {printf("Rs2: x%d\n", decimal(rs2));}
        if (*rd)
        {printf("Rd: x%d\n", decimal(rd));}
        if (*funct3 && (type_name == "R"))
        {printf("Funct3: %d\n", decimal(funct3));}
        if (*funct7 && (type_name == "R"))
        {printf("Funct7: %d\n", decimal(funct7));}
        if (*imm)
        {printf("Immediate: %d (or 0x%x)\n", decimal(imm), decimal(imm));};
    }
    // We need to populate the out buffer (ID/EXE) to fulfill the decode stage
    id_exe_buffer->pc = if_id_buffer->pc;
    if (*rs1)
    {id_exe_buffer->rs1 = decimal(rs1); id_exe_buffer->read_data1 = reg_file[decimal(rs1)];} 
    if (*rs2)
    {id_exe_buffer->rs2 = decimal(rs2); id_exe_buffer->read_data2 = reg_file[decimal(rs2)];}
    if (*rd)
    {id_exe_buffer->rd = decimal(rd);}
    if (*imm)
    {id_exe_buffer->immediate = decimal(imm);};

    // We finally need to store the actual operation we decoded.
    // Depending on our implementation, there are many ways to do this.
    // One is literally storing the name:
    //id_exe_buffer->instruction = name;

    // Actual datapaths use the ALUOp control signal so we can do that also based upon
    // the opcode, funct3, and funct7 values. 
    // We need to store the control signals as an array of binary values regardless. So
    // this is only natural.
    // This is assumedly very complicated so our current method will just fill the values
    // in based on human interpretation of the instruction type and name.

    if (type_name == "I")
    {
        // Set control signals for I-type instructions
        id_exe_buffer->control_signals[0] = 1; // RegWrite
        id_exe_buffer->control_signals[1] = 0; // ALUSrc
    }
    else if (type_name == "S")
    {
        // Set control signals for S-type instructions
        id_exe_buffer->control_signals[0] = 0; // RegWrite
        id_exe_buffer->control_signals[1] = 1; // ALUSrc
    }
    else if (type_name == "R")
    {
        // Set control signals for R-type instructions
        id_exe_buffer->control_signals[0] = 1; // RegWrite
        id_exe_buffer->control_signals[1] = 0; // ALUSrc
    }
    else if (type_name == "SB")
    {
        // Set control signals for SB-type instructions
        id_exe_buffer->control_signals[0] = 0; // RegWrite
        id_exe_buffer->control_signals[1] = 0; // ALUSrc
    }
    else if (type_name == "U")
    {
        // Set control signals for U-type instructions
        id_exe_buffer->control_signals[0] = 1; // RegWrite
        id_exe_buffer->control_signals[1] = 1; // ALUSrc
    }
    else if (type_name == "UJ")
    {
        // Set control signals for UJ-type instructions
        id_exe_buffer->control_signals[0] = 1; // RegWrite
        id_exe_buffer->control_signals[1] = 1; // ALUSrc
        id_exe_buffer->control_signals[2] = 0; // MemRead
        id_exe_buffer->control_signals[3] = 0; // MemWrite
        id_exe_buffer->control_signals[4] = 0; // Branch
        id_exe_buffer->control_signals[5] = 1; // Jump
    }

    // Garbage Collection (all dynamically allocated pointers)
    //delete[] rs1; delete[] rs2; delete[] rd; delete[] funct3; delete[] funct7;
     //delete[] imm; 
     delete[] imm1; delete[] imm2; delete[] imm3; delete[] imm4;
};

int decimal(const char* bin)
{
    int output = 0;
    int size = 0;

    const char* itr = bin;
    while(*itr)
    {
        itr++;
        size++;
    }
    int i = size - 1; // -1 to account for 0-indexing
    
    itr = bin; // reset iterator

    // If we are converting an immediate
    // AND check for a 1 in most significant bit
    if (((size == 12) || (size == 13) || (size == 21)) && (int_from_ascii(*bin) == 1))
    {
        // if so flip bits as we go and add 1
        {
            while(*itr && i >= 0)
            {
                int bit = !int_from_ascii(*itr); // flip bit
                output += (bit << i); // shift left by i (*2^i)
                itr++; i--;
            }
            output++;
            output *= -1;
        }
    }
    else
    {
        while(*itr && i >= 0)
        {
            int bit = int_from_ascii(*itr);
            output += (bit << i); // shift left by i (*2^i)
            itr++; i--;
        }
    }
    return output;
};

int int_from_ascii(char a)
{
    switch(int(a))
    {
        case (48):
            return 0;
        case (49):
            return 1;
        case (50):
            return 2;
        case (51):
            return 3;
        case (52):
            return 4;
        case (53):
            return 5;
        case (54):
            return 6;
        case (55):
            return 7;
        case (56):
            return 8;
        case (57):
            return 9;
        default:
            return -1;
    }
    
};

int string_comp(const char* a, const char* b)
{
    const char* itr_a = a;
    const char* itr_b = b;
    // while we have yet to hit a null terminator
    while(*itr_a && *itr_b)
    {
        if (*itr_a != *itr_b)
        {break;}
        itr_a++; itr_b++;
    }
    // if either are invalid (really only need 1 as same length) we iterated
    // through all of them without finding a mismatch.
    if (!(*itr_a) || !(*itr_b))
        return 1;
    return 0;
};

const char* get_type(const char* opcode)
{
    for (int i = 0; i < 13; i++)
    {
        if (string_comp(opcode, opcodes[i][0]))
            return opcodes[i][1];
    }
    return "NO OPCODE FOUND";
};

const char* get_name(const char* opcode, const char* funct3, const char* funct7)
{
    /* Realistically there should be some more intelligent way to do this than
    one monsterous switch statement composition but for now this is the most
    straightforward option. It hurts to look at for us too but at least its logically
    consistent.*/

    int m_opcode = decimal(opcode);
    int m_funct3 = decimal(funct3);
    int m_funct7 = decimal(funct7);

    switch (m_opcode)
    {
    case 3: // 0000011 (lb, lh, lw, ld, lbu , lhu, lwu)
        switch (m_funct3)
        {
        case 0: // 000
            return "lb";
            break;

        case 1: // 001
            return "lh";
            break;
        
        case 2: // 010
            return "lw";
            break;
        
        case 3: // 011
            return "ld";
            break;
        
        case 4: // 100
            return "lbu";
            break;
        
        case 5: // 101
            return "lhu";
            break;
        
        case 6: // 110
            return "lwu";
            break;
        
        default: // Not found
            break;
        }
        break;
    
    case 15: // 0001111 (fence, fencei)
        switch (m_funct3)
        {
        case 0: // 000
            return "fence";
            break;
        
        case 1: // 001
            return "fencei";
            break;
        
        default: // Not found
            break;
        }
        break;
    
    case 19: // 0010011 (addi, slli, slti, sltiu, xori, srli, srai, ori, andi)
        switch (m_funct3)
        {
        case 0: // 000
            return "addi";
            break;
        
        case 1: // 001
            return "slli";
            break;
        
        case 2: // 010
            return "slti";
            break;
        
        case 3: // 011
            return "sltiu";
            break;
        
        case 4: // 100
            return "xori";
            break;
        
        case 5: // 101 (srli and srai)
            switch (m_funct7)
            {
            
            case 0: // 0000000
                return "srli";
                break;
            
            case 32: // 0100000
                return "srai";
                break;
            
            default: // Not found
                break;
            }
        
        case 6: // 110
            return "ori";
            break;
        
        case 7: // 111
            return "andi";
            break;
        
        default: // Not found
            break;
        }
        break;
    
    case 23: // 0010111 (auipc)
        return "auipc";
        break;
    
    case 27: // 0011011 (addiw,slliw,srliw,sraiw)
        
        switch (m_funct3)
        {
        
        case 0: // 000
            return "addiw";
            break;
        
        case 1: // 001
            return "slliw";
            break;
        
        case 5: // 101 
            switch (m_funct7)
            {
            
            case 0: // 0000000
                return "srliw";
                break;
            
            case 32: // 0100000
                return "sraiw";
                break;
            
            default: // Not found
                break;
            }
            break;
        
        default: // Not found
            break;
        }
        break;
    
    case 35: // 0100011 (sb,sh,sw,sd)
        
        switch (m_funct3)
        {
        
        case 0: // 000
            return "sb";
            break;
        
        case 1: // 001
            return "sh";
            break;
        
        case 2: // 010
            return "sw";
            break;
        
        case 3: // 011
            return "sd";
            break;
        
        default: // Not found
            break;
        }
        break;
    
    case 51: // 0110011 (add,sub,sll,slt,sltu,xor,srl,sra,or,and)
        
        switch (m_funct3)
        {
        case 0: // 000 (add,sub)
            
            switch (m_funct7)
            {
            
            case 0: // 0000000
                return "add";
                break;
            
            case 32: // 0100000
                return "sub";
                break;
            
            default: // Not found
                break;
            }
            break;
        
        case 1: // 001
            return "sll";
            break;
        
        case 2: // 010
            return "slt";
            break;
        
        case 3: // 011
            return "sltu";
            break;
        
        case 4: // 100
            return "xor";
            break;
        
        case 5: // 101 (slr,sra)
            
            switch (m_funct7)
            {
            
            case 0: // 0000000
                return "slr";
                break;
            
            case 32: // 0100000
                return "sra";
                break;
            
            default: // Not found
                break;
            }
            break;
        
        case 6: // 110
            return "or";
            break;
        
        case 7: // 111
            return "and";
            break;
        
        default: // Not found
            break;
        }
        break;
    
    case 55: // 0110111 (lui)
        return "lui";
        break;
    
    case 59: // 0111011 (addw,subw,sllw,srlw,sraw)
        switch (m_funct3)
        {
            case 0: // 000
                switch (m_funct7)
                {
                
                case 0: // 0000000
                    return "addw";
                    break;
                
                case 32: // 0100000
                    return "subw";
                    break;
                
                default: // Not found
                    break;
                }
                break;
        case 1: // 001
            return "sllw";
            break;
        
        case 5: // 101 (srlw, sraw)
            switch (m_funct7)
                {
                
                case 0: // 0000000
                    return "srlw";
                    break;
                
                case 32: // 0100000
                    return "sraw";
                    break;
                
                default: // Not found
                    break;
                }
            break;
        }
        break;

    case 99: // 1100011 (beq,bne,blt,bge,bltu,bgeu)
        
        switch (m_funct3)
        {
        
        case 0: // 000
            return "beq";
            break;
        
        case 1: // 001
            return "bne";
            break;
        
        case 4: // 100
            return "blt";
            break;
        
        case 5: // 101
            return "bge";
            break;
        
        case 6: // 110
            return "bltu";
            break;
        
        case 7: // 111
            return "bgeu";
            break;

        default: // Not found
            break;
        }
        break;

    case 103: // 1100111 (jalr)
        return "jalr";
        break;

    case 111: // 1101111 (jal)
        return "jal";
        break;
    
    case 115: // 1110011 (ecall,ebrake,CSRRW,CSRRS,CSRRC,CSRRWI,CSRRSI,CSRRCI)
        switch (m_funct3)
        {
        case 0: // 000 (ecall, ebrake)
            switch (m_funct7)
            {
            case 0: // 0000000
                return "ecall";
                break;
            
            case 32: // 0100000
                return "ebrake";
                break;
            
            default: // Not found
                break;
            }
            break;
        
        case 1: // 001
            return "CSRRW";
            break;

        case 2: // 010
            return "CSRRS";
            break;

        case 3: // 011
            return "CSRRC";
            break;

        case 4: // 100
            return "CSRRWI";
            break;

        case 5: // 101
            return "CSRRSI";
            break;
        
        case 6: // 110
            return "CSRRCI";
            break;

        default: // Not found
            break;
        }
        break;

    default: // none of the above
        break;
    }
    return "ERROR"; // Never found an ins name
};

const char* get_opcode(const char* instruction)
{
    // We must check which type it is
    // First check opcode as it is the most consistant
    // opcode slots [6:0]
    return gf_imm(instruction, 6, 0);
};

char* gf_rs1(const char* instruction)
{
    // get [19:15] (20th to 16th place)
    return gf_imm(instruction, 19, 15);
};

char* gf_rs2(const char* instruction)
{
    // get [24:20] (25th to 21st place)
    return gf_imm(instruction, 24, 20);
};

char* gf_rd(const char* instruction)
{
    // get [11:7] (12th to 7th place)
    return gf_imm(instruction, 11, 7);
};

char* gf_funct3(const char* instruction)
{
    // get [14:12] (15th to 13th place)
    return gf_imm(instruction, 14, 12);
};

char* gf_funct7(const char* instruction)
{
    // get [31:25] (32nd to 26th place)
    return gf_imm(instruction, 31, 25);
};

char* gf_imm(const char* instruction, int start, int end)
{
    // get [start:end] (start to end only)

    // Ptr to beginning of dynamically alloc output char array
    char* output = new char[start - end + 1];
    
    // After allocation, we have to flip the indices to read
    // left to right since we read mach inst right to left
    start = 31 - start;
    end = 31 - end;

    // Iterator for output char array
    char* output_ptr = output;
    // Iterator for input instruction char array
    const char* ins_ptr = (instruction + start);

    // while
    // ins_ptr is valid (not 0 or null terminator)
    // and ins_ptr is not past end of the sample
    while (ins_ptr && (ins_ptr <= (instruction + end)))
    {
        *output_ptr = *ins_ptr;
        ins_ptr++;
        output_ptr++;
    }
    *output_ptr = '\0';
    return output;
    /*
    // get the end-index all the way to the beginning
    int temp = instruction % (10^end); 
    // chop off 1 before the start all the way to the beginning
    return (temp / 10^(start - 1)); 
    */ 
};
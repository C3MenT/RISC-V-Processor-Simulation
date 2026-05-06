#include <stdio.h>
#include <string.h>
#include "../header/decoder.h"

void Decode(IF_ID_buffer *if_id_buffer, ID_EXE_buffer *id_exe_buffer, bool debug)
{
    // Extract the instruction from the IF/ID buffer
    const char* instruction = if_id_buffer->instruction;

    if (FLUSH == 2) // if we must flush up to the ID
    {
        // clear the instruction since we got an incorrect one
        instruction = "00000000000000000000000000000000\0";
        FLUSH--;
    }


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

    if (debug)
    {
        if (pipeline)
        {
            static int inst_index = -1;
            inst_index++;
            printf("\nDECODE STAGE (%d) ===============================\n", inst_index);
        }
        else
            printf("\nDECODE STAGE ===============================\n");

        // Get name of instruction
        name = get_name(opcode, funct3, funct7);
        // Print Sequence (Now for debug purposes)
        printf("Instruction Type: %s\n", type_name);
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
    // Pass along normal next pc value to potentially store for jump instructions
    id_exe_buffer->pc = if_id_buffer->pc;
    if (*rs1)
    {id_exe_buffer->rs1 = decimal(rs1); id_exe_buffer->read_data1 = rf[decimal(rs1)];} 
    if (*rs2)
    {id_exe_buffer->rs2 = decimal(rs2); id_exe_buffer->read_data2 = rf[decimal(rs2)];}
    if (*rd)
    {id_exe_buffer->rd = decimal(rd);}
    if (*imm)
    {id_exe_buffer->immediate = decimal(imm);};

    // We finally need to store the actual operation we decoded.
    // Depending on our implementation, there are many ways to do this.
    // One is literally storing the name:
    //id_exe_buffer->instruction = name;

    ControlUnit(id_exe_buffer, type_name, opcode, funct3, funct7, debug); // this will populate the control_signals global variable based on the instruction type

    // Garbage Collection (all dynamically allocated pointers)
    //delete[] rs1; delete[] rs2; delete[] rd; delete[] funct3; delete[] funct7;
     //delete[] imm; 
     delete[] imm1; delete[] imm2; delete[] imm3; delete[] imm4; //delete[] opcode; delete[] type_name; delete[] funct3; delete[] funct7;

     if(debug)
     {
        id_exe_buffer->print_buffer(); 
        printf("============================================\n");
     }
};

void ControlUnit(ID_EXE_buffer* id_exe_buffer, const char* type_name, const char* opcode, const char* funct3, const char* funct7, bool debug)
{
    // Actual datapaths use the ALUOp control signal so we can do that also based upon
    // the opcode, funct3, and funct7 values. 
    // We need to store the control signals as an array of binary values regardless. So
    // this is only natural.
    // This is assumedly very complicated so our current method will just fill the values
    // in based on human interpretation of the instruction type and name.

    // ALU Op is a bit more nuanced
    // It is an integer value representing the actual 2-bit ALU Op control signal,
    // where 0 is Load/Store, 1 is Branch, 2 is R-type, and 3 is I-type.
    
    // Regardless, all will begin flipped off and we will turn on the ones we need for each instruction type
    RegWrite = 0;
    id_exe_buffer->RegWrite = 0;
    Branch = 0;
    id_exe_buffer->Branch = 0;
    ALUSrc = 0;
    id_exe_buffer->ALUSrc = 0;
    MemWrite = 0;
    id_exe_buffer->MemWrite = 0;
    MemtoReg = 0;
    id_exe_buffer->MemtoReg = 0;
    MemRead = 0;
    id_exe_buffer->MemRead = 0;
    Jump = 0;
    id_exe_buffer->Jump = 0;
    
    //id_exe_buffer->PCSrc = 0;
    // We reset pc_target here because of jals (see UJ clause)
    id_exe_buffer->pc_target = 0;

    // we make an ALUOp "signal" (var) to pass to the ALU Control "Unit" (function)
    int ALUOp[2] = {0, 0};

    // There are special I types that need extra control signal checks to support
    if (type_name == "I")
    {
        // Set control signals for I-type instructions
        RegWrite = 1;
        id_exe_buffer->RegWrite = 1;
        ALUSrc = 1;
        id_exe_buffer->ALUSrc = 1;
        
        if (decimal(opcode) == 3) // if we are doing a load instruction
        {
            // Loads additionally need MemtoReg set and MemRead set
            MemtoReg = 1;
            id_exe_buffer->MemtoReg = 1;
            MemRead = 1;
            id_exe_buffer->MemRead = 1;
            // Loads always need an ADD operation so ALUOp becomes 00
            // ALUOp is already 00
        }
        else if (decimal(opcode) == 103) // If this happens to be JALR
        {
            // Jalr is a little weird but understandable
            // Jalr writes current pc to rd like Jal and it needs an ADD operation
            // to add the immediate to whatever is in rs1 rather than pc (logically an address)
            // It then should set the branch target to this result
            Jump = 1;
            id_exe_buffer->Jump = 1;
            // 3 - Jalr
            id_exe_buffer->PCSrc = 3; 

            // ALUOp is (10) for JALR since it has a funct3
            // since the ALU control ADDs (offset from addr) it needs an add operation
            // instead of determining it now we use the funct3 being 000 to let ALU Control
            // determine it after this
            ALUOp[0] = 1; ALUOp[1] = 0; 
        }
        else
        {
            ALUOp[0] = 1; ALUOp[1] = 0; // ALUOp is 2 for ALU I-type instructions since the ALU control signals are determined by the funct3 field of the instruction
        }
    }
    else if (type_name == "S")
    {
        // Set control signals for S-type instructions
        ALUSrc = 1;
        id_exe_buffer->ALUSrc = 1;
        MemWrite = 1;
        id_exe_buffer->MemWrite = 1;
        // ALUOp is 0 for store instructions since the ALU just needs to perform an addition to calculate the memory address
    }
    else if (type_name == "R")
    {
        // Set control signals for R-type instructions
        RegWrite = 1;
        id_exe_buffer->RegWrite = 1;
        // ALUOp is 2 for R-type instructions since the ALU control signals are determined by the funct3 and funct7 fields of the instruction
        ALUOp[0] = 1; ALUOp[1] = 0;
    }
    else if (type_name == "SB")
    {
        // Set control signals for SB-type instructions
        Branch = 1;
        id_exe_buffer->Branch = 1;

        // 1 - Branch (PC + imm)
        id_exe_buffer->PCSrc = 1;
        // Branch predictors would use their own adder to get the offset like in Jal so we simulate that here
        id_exe_buffer->pc_target = id_exe_buffer->pc - 4 + id_exe_buffer->immediate;

        // ALUOp is 1 for branch instructions since the ALU just needs to perform a subtraction to compare the two register values
        ALUOp[0] = 0; ALUOp[1] = 1;  
    }
    // U Types are strange
    // There are only lui "load upper immediate" and auipc "add upper imm to pc"
    // Upper immediate instructions are just longer immediate instructions that 
    // do not have a funct3 field either.
    // We will support load and add to pc with an add ALU control as if it is a mem op
    // auipc will need extra support which will be left as TODO if ever we feel like adding it
    else if (type_name == "U")
    {
        // Set control signals for U-type instructions
        RegWrite = 1;
        id_exe_buffer->RegWrite = 1;
        ALUSrc = 1;
        id_exe_buffer->ALUSrc = 1;
        // ALUOp (0 for U-type) 
        // ALUOp is already 00
    }
    // Jal is the only UJ type
    // Jal is special in that it does not use the ALU; it has a dedicated adder.
    // Jal like jalr stores current pc in the rd reg but
    // it calculates an offset from current pc, setting pc to the result.
    else if (type_name == "UJ")
    {
        // Set control signals for UJ-type instruction
        // to store PC in write back
        RegWrite = 1;
        id_exe_buffer->RegWrite = 1;
        // ALU isn't used
        // ALUSrc = X (Don't care)
        // assert jump
        Jump = 1;
        id_exe_buffer->Jump = 1;

        id_exe_buffer->PCSrc = 2; // 2 for jal
        // In leu of a personal adder unit we just calculate the target here.
        // It will propagate along until the wb or mem stage.
        // We add to next pc in the id_exe_buffer since we just got it from the if_id_buffer
        // and therefore, it is the correct PC + 4 value for this stage. We subtract 4 to get
        // the pc corresponding to this instruction.
        
        if (pipeline)
        {
            // in the pipelined case, we must update pc immediately and compensate with stalls
            // setting it here eschews any need to check PCSrc in Fetch which is what actually happens
            pc = id_exe_buffer->pc - 4 + id_exe_buffer->immediate;
            // We only "flush" the IF stage
            // Since we jump early before decode is finished and the implementation executes
            // stages backwards, there is no need to actually do anything since Fetch has not
            // happened and goten yet anything in reality.
            FLUSH = 1; 
            // we also have to check for at least 3 stages (2 with forwarding if anything needs 'rd')
            if (debug)
                std::cout << "Jumping to " << pc << std::endl;
        }
        else
            id_exe_buffer->pc_target = id_exe_buffer->pc - 4 + id_exe_buffer->immediate;
        
        // ALUOp does not matter ALU isn't used
    }
    // set the actual ALU control signals based on the ALUOp and funct3/funct7 values
    ALUControl(id_exe_buffer, ALUOp, decimal(funct3), decimal(funct7));
};

void ALUControl(ID_EXE_buffer* id_exe_buffer,int alu_op[2], int funct3, int funct7)
{
    // ALU Control is determined by the ALUOp control signal as well as the funct3 and funct7 fields of the instruction
    // It is a 4-bit control signal that determines the actual operation the ALU performs.
    // For R-type instructions, the ALUOp is 2, and the funct3 and funct7 fields determine the specific operation (e.g., add, sub, and, or, etc.).
    // For I-type instructions, the ALUOp is 3, and the funct3 field determines the specific operation (e.g., addi, slti, xori, etc.).
    // For load/store instructions, the ALUOp is 0, and the ALU performs an addition to calculate the memory address.
    // For branch instructions, the ALUOp is 1, and the ALU performs a subtraction to compare the two register values.

    // This function will set the global alu_ctrl variable based on these inputs for use in the execute stage.
    // We start all bits flipped off and then turn on the ones we need for each instruction type based on the inputs.

    alu_ctrl[0] = 0; alu_ctrl[1] = 0; alu_ctrl[2] = 0; alu_ctrl[3] = 0; // default to AND (all 0s)
    id_exe_buffer->ALU_CTRL[0] = 0; id_exe_buffer->ALU_CTRL[1] = 0; id_exe_buffer->ALU_CTRL[2] = 0; id_exe_buffer->ALU_CTRL[3] = 0;

    if (alu_op[0] == 0 && alu_op[1] == 0) // Load/Store
    {
        // ALU performs addition to calculate memory address
        alu_ctrl[2] = 1; // ADD (0010)
        id_exe_buffer->ALU_CTRL[2] = 1;
    }
    else if (alu_op[0] == 0 && alu_op[1] == 1) // Branch
    {
        // ALU performs subtraction to compare the two register values
        alu_ctrl[1] = 1;// SUB (0100)
        id_exe_buffer->ALU_CTRL[1] = 1;
    }
    else if (alu_op[0] == 1 && alu_op[1] == 0) // "R-type"
    {
        switch (funct3)
        {
            case 0: // ADD or SUB
                if (funct7 == 32) // SUB
                {
                    alu_ctrl[1] = 1; // SUB (0100)
                    id_exe_buffer->ALU_CTRL[1] = 1;
                }
                else // ADD
                {
                    alu_ctrl[2] = 1; // ADD (0010)
                    id_exe_buffer->ALU_CTRL[2] = 1;
                }
                break;
            case 7: // AND
                // AND (0000)
                break;
            case 6: // OR
                alu_ctrl[0] = 1; // OR (1000)
                id_exe_buffer->ALU_CTRL[0] = 1;
                break;
            case 4: // XOR
                alu_ctrl[0] = 1; alu_ctrl[1] = 1; // XOR (1100)
                id_exe_buffer->ALU_CTRL[0] = 1;
                id_exe_buffer->ALU_CTRL[1] = 1;
                break;
        }
    }
}

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
    const char* opcodes[13][2] = {
        {"0000011", "I"},
        {"0001111", "I"},
        {"0010011", "I"},
        {"0010111", "U"},
        {"0011011", "I"},
        {"0100011", "S"},
        {"0110011", "R"},
        {"0110111", "U"},
        {"0111011", "R"},
        {"1100011", "SB"},
        {"1100111", "I"},
        {"1101111", "UJ"},
        {"1110011", "I"}
    };

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
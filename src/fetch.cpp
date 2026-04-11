#include <stdio.h>
#include "../header/fetch.h"

int Fetch(FILE *file, IF_ID_buffer *if_id_buf)
{
    /*
        Fetch reads an input file of machine instruction lines.
        Each line is a 32-bit instruction represented as a string of 1s and 0s.
        We read in the instruction as a string and store it in the input buffer
        along with the current program counter value for use in the decode stage.

        Each line is enumerated as an instruction read using the pc value. 
        For example, if pc is 0, we read in the 0th instruction, 
        if pc is 4, we read in the 1st instruction, etc.
    */
    // get line number to read based on current pc value
    int line = pc / 4; 
    // calculate next pc value for the next instruction to read in the next cycle
    int next_pc = pc + 4;
    // store the next pc value in the buffer 
    if_id_buf->pc = next_pc; 

    // read until first whitespace, store in instruction field of IF/ID buffer
    for (int i = 0; i < line; i++)
    {
        // skip lines until we get to the line we want to read
        fscanf(file, "%*[^\n]\n");
    }
    return fscanf(file, "%s", if_id_buf->instruction); 
    // fscanf returns the number of items successfully read, 
    // so it will return 0 if we have reached the end of the input stream 
    // and there are no more instructions to read.
}
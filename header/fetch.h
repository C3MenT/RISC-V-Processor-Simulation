#pragma once

/// @brief Function fetches a 32-bit machine code instruction from standard input
/// @param file pointer to the input file
/// @param out_buffer pointer to the output buffer (IF/ID)
/// @return the number of items successfully read (aka still reading), 
/// or 0 if end of input stream was reached
int Fetch(FILE* file, IF_ID_buffer* if_id_buf);
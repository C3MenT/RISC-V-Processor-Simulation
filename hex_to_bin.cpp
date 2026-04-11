/*Simple Script to convert instructions in a text file from hexadecimal to binary*/

#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[])
{
    if (argv[1] == nullptr || argv[2] == nullptr)
    {
        std::cerr << "Usage: hex_to_bin <input_file> <output_file>" << std::endl;
        return 1;
    }
    
    std::ifstream infile(argv[1]);
    std::ofstream outfile(argv[2]);

    std::string hex;
    while (infile >> hex)
    {
        unsigned long long int num = std::stoull(hex, nullptr, 16);
        std::string binary = "";
        for (int i = 31; i >= 0; i--)
        {
            binary += ((num >> i) & 1) ? '1' : '0';
        }
        outfile << binary << std::endl;
    }

    infile.close();
    outfile.close();

    return 0;
}
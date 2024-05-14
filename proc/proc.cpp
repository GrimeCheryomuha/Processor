#include "proc.hpp"

int main (int argc, char *argv[]) {

    string file_name = NULL;
    
    if (argc >= 2) file_name = argv[1];

    Processor cpu;

    cpu.readCode    (file_name);

    return 0;
}
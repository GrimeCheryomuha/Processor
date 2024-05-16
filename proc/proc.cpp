#include "proc.hpp"

int main (int argc, char *argv[]) {

    std::string file_name;

    if (argc >= 2) file_name = argv[1];

    Processor cpu;

    cpu.readCode    (file_name);
    cpu.runCpu      ();

    return 0;
}
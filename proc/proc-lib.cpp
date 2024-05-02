#include "proc.hpp"

Processor::Processor ()  : 
    regs(REG_SIZE, 0), code(NULL), ram(new double[RAM_SIZE]) {

        for (int i = 0; i < STACK_CAPACITY; i++){

            stk.push(0);
            if (i < CALL_STACK_CAPACITY) 
                call_stk.push(0);
        }
    }

Processor::~Processor () {

    delete[] ram;
}

void    Processor::readCode (const string Fname) {
    
    ifstream input_file (Fname);

    if (!input_file.is_open()) return;

    size_t filesize = getfileSize (Fname);    

    code_size = static_cast<int> (filesize / sizeof(Processor) - CODE_SHIFT);

    code = new char[filesize];

    input_file.read(code, filesize);
    code += CODE_SHIFT;

    input_file.close();
}

int     Processor::getArg   (int cmd) {

    int arg = 0;

    if (cmd & static_cast<int> (Masks::MASK_REG)) {

        int reg = code[ip++];
        arg += regs[reg]; 
    }

    if (cmd & static_cast<int> (Masks::MASK_IMM))
        arg += code[ip++];

    if (cmd & static_cast<int> (Masks::MASK_RAM))
        arg = ram[arg];

    *arg_p = arg;
}

size_t getfileSize (const string file_name) {

    ifstream input_file (file_name); 

    input_file.seekg (0, ios::end);
    size_t filesize = input_file.tellg();
    input_file.seekg (0, ios::beg);

    return filesize;
}
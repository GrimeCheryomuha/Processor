#include "proc.hpp"

Processor::Processor ()  : 
    regs(REG_SIZE, 0), code(NULL), ram(new double[RAM_SIZE]) {}

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

double  Processor::stkPop   (double* arg) {
    
    *arg = stk.top ();
    stk.pop ();
    return *arg;
}

void    Processor::getArg   (int cmd, double *arg_p) {

    double arg = 0;

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

void    Processor::runCpu       () {

    while (ip < code_size) {

        char cmd = code[ip++];

#define DEF_CMD(name, num, arg, code)  \
    case CMD_##name:                   \
        getArg (cmd, &arg);            \
        code;                          \
        break;                         

        switch (cmd & CMD) {

            #include "cmd.hpp"

        default:

            cerr << "Cmd error" << endl;
        }
    }
}

size_t getfileSize (const string file_name) {

    ifstream input_file (file_name); 

    input_file.seekg (0, ios::end);
    size_t filesize = input_file.tellg();
    input_file.seekg (0, ios::beg);

    return filesize;
}
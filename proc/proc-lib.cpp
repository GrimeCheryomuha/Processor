#include "proc.hpp"

Processor::Processor ()  : 
    regs(REG_SIZE, 0), code(NULL), ram(new double[RAM_SIZE]), errors(8, 0) {}

Processor::~Processor () {

    delete[] ram;
}

void    Processor::readCode (const string Fname) {
    
    if (Fname.length() == 0) {

        errors[static_cast<int> (Errors::NO_FILE)] += 1;
        return;
    }

    ifstream input_file (Fname);

    if (!input_file.is_open()) {

        errors[static_cast<int> (Errors::WRONG_OPEN_FILE)] += 1;
        return;
    }

    size_t filesize = getfileSize (Fname);    

    code_size = static_cast<int> (filesize / sizeof(Processor) - CODE_SHIFT);

    code = new char[filesize];

    input_file.read(code, filesize);
    code += CODE_SHIFT;

    input_file.close();
}

double  Processor::stkPop   (double* arg) {

    if (arg == NULL) {

        errors[static_cast<int> (Errors::NULL_PTR_ARG)] += 1;
        return 0;
    }
    
    *arg = stk.top ();
    stk.pop ();
    return *arg;
}

void    Processor::getArg   (int cmd, double *arg_p) {

    if (arg_p == NULL) {

        errors[static_cast<int> (Errors::NULL_PTR_ARG)] += 1;
        return;
    }
    
    int arg = 0;

    if (cmd & static_cast<int> (Masks::MASK_REG)) {

        int reg = code[ip++];
        if(reg <= 0 or reg >= REG_SIZE) {

            errors[static_cast<int> (Errors::WRONG_REG)] += 1;
            return;
        }
        arg += regs[reg]; 
    }

    if (cmd & static_cast<int> (Masks::MASK_IMM))
        arg += code[ip++]; 
    
    if (cmd & static_cast<int> (Masks::MASK_RAM)) {

        if (arg >= RAM_SIZE) {

            errors[static_cast<int> (Errors::WRONG_RAM_ADRESS)] += 1;
            return;
        }
        arg = ram[arg];
    }

    *arg_p = arg;
}

// void    Processor::runCpu       () {

//     while (ip < code_size) {

//         char cmd = code[ip++];

// #define DEF_CMD(name, num, arg, code)  \
//     case CMD_##name:                   \
//         getArg (cmd, &arg);            \
//         code;                          \
//         break;                         

//         switch (cmd & CMD) {

//             #include "cmd.hpp"

//         default:

//             cerr << "Cmd error" << endl;
//         }
//     }
// }

size_t      Processor::getfileSize (const string file_name) {

    if (file_name.length() <= 0) {

        errors[static_cast<int> (Errors::NO_FILE)] += 1;
        return 0;
    }

    ifstream input_file (file_name); 

    if (!input_file.is_open()) {

        errors[static_cast<int> (Errors::WRONG_OPEN_FILE)] += 1;
        return 0;
    }

    input_file.seekg (0, ios::end);
    size_t filesize = input_file.tellg();
    input_file.seekg (0, ios::beg);

    input_file.close();

    return filesize;
}
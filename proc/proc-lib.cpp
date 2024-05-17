#include "proc.hpp"

/**
 * @brief Construct a new Processor:: Processor object and clears ram and reg
 *
 */
Processor::Processor () : ip (0), code_size (0) {

    for (int i = 0; i < RAM_SIZE; i++) ram[i] = 0;
    for (int i = 0; i < REG_SIZE; i++) regs[i] = 0;
}

/**
 * @brief Destroy the Processor:: Processor object and signify that execution was completed
 *
 */
Processor::~Processor () {

    std::cerr << "\n--------------------\nlikvidirovan\n" << std::endl;

}

void    Processor::readCode (const std::string Fname) {

    /// checks if no string given
    if (Fname.length() == 0) {

        errors[Errors::NO_FILE]++;
        return;
    }

    std::ifstream input_file (Fname, std::ios::binary);

    /// error during opening file
    if (!input_file.is_open()) {

        errors[Errors::WRONG_OPEN_FILE]++;
        return;
    }

    /// disables space tab and alike skip
    input_file.unsetf (std::ios::skipws);

    code_size = getfileSize (input_file); ///< gets size of file in bytes

    code.resize (code_size + 1); /// reserves + 1 to store final hlt (just a safety feature)
    input_file.read (reinterpret_cast<std::ifstream::char_type*> (&code.front ()), code_size); /// copies to vector
    code.back () = '\0';///places final hlt

    input_file.close();
}

void    Processor::stkPush  (int arg) {

    stk.push(arg);

    if (stk.size() > STACK_CAPACITY) {

        errors[static_cast<int> (Errors::STACK_OVERFLOW)] += 1;
        return;
    }
}

void    Processor::stkCallPush (int arg) {

    call_stk.push(arg);

    if (call_stk.size() > CALL_STACK_CAPACITY) {

        errors[static_cast<int> (Errors::STACK_OVERFLOW)] += 1;
        return;
    }
}

int     Processor::stkPop   () {

    if (stk.size() == 0)
        errors[static_cast<int> (Errors::EMPTY_STACK)] += 1;

    int arg = stk.top ();

    stk.pop ();
    return arg;
}

int     Processor::stkCallPop () {

    if (call_stk.size() == 0)
        errors[static_cast<int> (Errors::EMPTY_CALL_STACK)] += 1;

    int arg = call_stk.top ();

    call_stk.pop ();
    return arg;
}

int*    Processor::getArg   (char cmd) {

    if (cmd & Masks::MASK_RAM) { ///if arg is ram

        if (cmd & Masks::MASK_REG) { /// ram adress is reg

            int* retVal = ram + *(regs + *((int*)(code.data () + ip + 1)));
            ip += sizeof (int);
            return retVal;
        }

        if (cmd & Masks::MASK_IMM) { /// ram adress is imm


            int* retVal = ram + *((int*)(code.data () + ip + 1));
            ip += sizeof (int);
            return retVal;
        }
    }
    else if (cmd & Masks::MASK_REG) { /// arg is reg

        int* retVal = regs + *((int*)(code.data () + ip + 1));
        ip += sizeof (int);
        return retVal;
    }
    else if (cmd & Masks::MASK_IMM) { /// arg is imm or ip

        int* retVal = (int*)(code.data () + ip + 1);
        ip += sizeof (int);
        return retVal;
    }
    else {

        /// cry for help @ 40 hours no sleep
        std::cout << "if u are reading this than i have given up at processing this error (error is wrong arg type)" << std::endl;
        exit (__LINE__);
    }

    return NULL;
}

void    Processor::runCpu       () {

    for (ip = CODE_SHIFT; ip < code_size; ip++) {

        int tmp = 0; /// temp var
        int arg1 = 0, arg2 = 0; /// arg1 and 2 for comparator jumps, add, mul, sub, div
        int* arg = NULL; /// ptr to arg for push pop

        switch ((code[ip]) & (static_cast<char>(Masks::MASK_CMD))) {

        #define DEF_CMD(name, cmd, has_arg, ...)                                \
            case cmd:                                                           \
                if (has_arg == 1) arg = getArg (code[ip]);                      \
                if (has_arg == 2) arg = getArg ((code[ip]) | (Masks::MASK_IMM)); /*a quick fix that reads ip because jumps don't have MASK_CMD bit on, but has_arg=2*/\
                __VA_ARGS__                                                     \
                break;                                                          \

        #include "../lib/cmd.hpp"

        #undef DEF_CMD
        default:

            std::cout << "Cmd error" << std::endl;
            exit (-1);
        }
    }
}

bool    Processor::thereAreErrors   () {

    for (int i : errors) {

        if (i > 0) return 0;
    }

    return 1;
}

size_t Processor::getfileSize (std::ifstream& input_file) {

    if (!input_file.is_open()) {

        errors[Errors::WRONG_OPEN_FILE]++;;
        return 0;
    }

    input_file.seekg (0, std::ios::end);
    size_t filesize = input_file.tellg();
    input_file.seekg (0, std::ios::beg);

    return filesize;
}
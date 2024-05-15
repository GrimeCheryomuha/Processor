#include "proc.hpp"

Processor::Processor ()  :
    code(NULL), errors(11, 0) {}

Processor::~Processor () {

    cout << "Destructure" << endl;
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

// Здесь вместо указателя можно юзать ссылки ради ржаки
void    Processor::getArg   (int cmd, int *arg_p) {

    if (arg_p == NULL) {

        errors[static_cast<int> (Errors::NULL_PTR_ARG)] += 1;
        return;
    }

    if (code == NULL) {

        errors[static_cast<int> (Errors::NULL_PTR_CODE)] += 1;
        return;
    }

    int arg = 0;

    if (cmd & static_cast<int> (Masks::MASK_REG)) {

        // !!!!!!!!!!!!!!!!!!! ЕСЛИ КОД У ТЕБЯ ЧАР МАССИВ ТО ТУТ ХУЙНЯ ЗАПИШЕТСЯ (ТОЛЬКО ПЕРВЫЙ БАЙТ ИНТА). ТУТ КАСТ НУЖЕН
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

void    Processor::runCpu       () {

    while (ip < code_size) {

        int cmd = code[ip++];

<<<<<<< HEAD
#define DEF_CMD(name, num, arg, ...)        \
    case CMD_##name:                        \
        __VA_ARGS__;                        \
        break;

=======
>>>>>>> refs/remotes/origin/main
        switch (cmd & CMD_MASK) {

#define DEF_CMD(name, num, arg, code)  \
    case CMD_##name:                   \
        if (arg) getArg (cmd, &arg);   \
        code                           \
        break;
            
            #include "cmd.hpp"

        #undef DEF_CMD
        default:

            cerr << "Cmd error" << endl;
        }
    }
}

bool    Processor::thereAreErrors   () {

    for (int i : errors) {
       
        if (i > 0) return 0;
    }

    return 1;
}

size_t Processor::getfileSize (const string file_name) {

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

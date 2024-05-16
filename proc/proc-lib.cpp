#include "proc.hpp"

Processor::Processor ()  :
    ip (0), code_size (0) {}

Processor::~Processor () {

    std::cout << "likvidirovan\n" << std::endl;

}

/// @warning May be buggy, experimental solution
void    Processor::readCode (const std::string Fname) {

    if (Fname.length() == 0) {

        errors[Errors::NO_FILE]++;
        return;
    }

    std::ifstream input_file (Fname, std::ios::binary);

    if (!input_file.is_open()) {

        errors[Errors::WRONG_OPEN_FILE]++;
        return;
    }

    input_file.unsetf (std::ios::skipws);

    size_t code_size = getfileSize (input_file);

    code.resize (code_size + CODE_SHIFT + 1);
    input_file.read (reinterpret_cast<std::ifstream::char_type*> (&code.front ()), code_size);
    code.back () = '\0';

    for (auto i : code) {

        std::ofstream test_file ("kys.test.file", std::ios::binary);
        test_file << i;
    }

    input_file.close();

    exit (__LINE__);
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

        switch (cmd & static_cast<int>(Masks::MASK_CMD)) {

        #define DEF_CMD(name, num, has_arg, ...)    \
            case CMD_ ## name:                        \
                __VA_ARGS__                         \
                break;

        #include "../lib/cmd.hpp"

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

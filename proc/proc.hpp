#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include <cmath>
#include <stack>
#include <fstream>
#include <cassert>

#define SIGNATURE "CP01"

class Processor {

public:

    enum Reg {

        r0x = 0,
        rax,
        rbx,
        rcx,
        rdx,
    };

    enum Masks {

        MASK_RAM = 1<<7,
        MASK_REG = 1<<6,
        MASK_IMM = 1<<5,
        MASK_CMD = (1<<5) - 1
    };

    enum Errors {

        OK                  =    0,
        WRONG_OPEN_FILE,
        STACK_OVERFLOW,
        WRONG_RAM_ADRESS,
        WRONG_REG,
        EMPTY_STACK,
        EMPTY_CALL_STACK,
        NO_FILE,
        NULL_PTR_ARG,
        NULL_PTR_CODE,
        WRONG_JMP_IP,
        ERR_TYPE_CNT /// Эта штука должна быть последней чтобы показывать суммарное кол-во типов ошибок чтобы выделялся правильный массив
    };

    Processor ();

    void    readCode        (const std::string input_file_name);
    void    checkSign       ();
    int*    getArg          (char cmd);
    void    runCpu          ();

    size_t  getfileSize     (std::ifstream& input_file);
    bool    thereAreErrors   ();

    int     stkPop          ();
    int     stkCallPop      ();
    void    stkPush         (int arg);
    void    stkCallPush     (int arg);

    ~Processor ();

private:

    static const int CODE_SHIFT = 4;
    static const int REG_SIZE = 5;
    static const int RAM_SIZE = 121;
    static const size_t STACK_CAPACITY = 16;
    static const size_t CALL_STACK_CAPACITY = 8;

    enum def_commands {

        #define DEF_CMD(name, num, arg, ...) \
            CMD_##name = num,\

        #include "../lib/cmd.hpp"

        #undef DEF_CMD
    };

    int ip = 0;
    int code_size = 0;

    /// Мб либо свапнуть стек на пару статик массив + указатель на верх стека, либо убрать лимит по размеру
    /// пока второе
    std::stack<int> stk;
    std::stack<int> call_stk;
    std::vector<char> code;

    int regs[REG_SIZE] = {0};
    int errors[ERR_TYPE_CNT] = {0};
    int ram[RAM_SIZE] = {0};
};

#define ERRORCHECK if (thereAreErrors()) return;

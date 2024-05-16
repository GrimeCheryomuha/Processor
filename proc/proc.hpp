#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include <cmath>
#include <stack>
#include <fstream>

using namespace std;

enum class Reg {

    r0x = 0,
    rax = 1,
    rbx = 2,
    rcx = 3,
    rdx = 4,
};

enum class Masks {

    MASK_RAM = 1<<7,
    MASK_REG = 1<<6,
    MASK_IMM = 1<<5,
};

enum class Errors {

    OK                  =    0,
    WRONG_OPEN_FILE     =    1,
    STACK_OVERFLOW      =    2,
    WRONG_RAM_ADRESS    =    3,
    WRONG_REG           =    4,
    EMPTY_STACK         =    5,
    EMPTY_CALL_STACK     =    6,
    NO_FILE             =    7,
    NULL_PTR_ARG        =    8,
    NULL_PTR_CODE       =    9,
    WRONG_JMP_IP        =   10,
};

const size_t STACK_CAPACITY = 16;
const size_t CALL_STACK_CAPACITY = 8;

const int CMD_MASK = 0x000000FF;

const int CODE_SHIFT = 4;

const int REG_SIZE = 5;
const int RAM_SIZE = 121;

class Processor {

public:

    Processor ();

    void    readCode        (const string inptut_file);
    void    getArg          (int cmd, int* arg_p);
    void    runCpu          ();

    size_t  getfileSize     (const string file_name);
    bool    thereAreErrors   ();

    int     stkPop          ();
    int     stkCallPop      ();
    void    stkPush         (int arg);
    void    stkCallPush     (int arg);

    ~Processor ();

private:

    enum def_commands {

        #define DEF_CMD(name, num, arg, ...) \
            CMD_##name = num,\

        #include "../lib/cmd.hpp"

        #undef DEF_CMD
    };

    int ip = 0;
    int code_size = 0;

    stack<int> stk;
    stack<int> call_stk;

    int regs[REG_SIZE];

    char* code;

    vector <int> errors;

    int ram[RAM_SIZE];
};

#define ERRORCHECK if (thereAreErrors()) return;

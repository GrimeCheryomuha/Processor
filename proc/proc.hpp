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

    OK                  = 0,
    WRONG_OPEN_FILE     = 1,
    STACK_OVERFLOW      = 2,
    WRONG_RAM_ADRESS    = 3,
    WRONG_REG           = 4,
    EMPTY_STACK         = 5,
    EMTY_CALL_STACK     = 6,
    NO_FILE             = 7,
    NULL_PTR_ARG        = 8
};

const size_t STACK_CAPACITY = 16;
const size_t CALL_STACK_CAPACITY = 8;

const int CODE_SHIFT = 4;

const int REG_SIZE = 5;
const int RAM_SIZE = 121;

class Processor {

public:

    Processor ();

    void    readCode    (const string inptut_file);
    void    getArg      (int cmd, double* arg_p);
    void    runCpu      ();

    size_t getfileSize  (const string file_name);

    double  stkPop      (double *arg);

    ~Processor ();

private:

    int ip = 0;
    int code_size = 0;

    stack<int> stk;
    stack<int> call_stk;

    int regs[REG_SIZE];

    char* code;

    vector <int> errors;

    int ram[RAM_SIZE];
};



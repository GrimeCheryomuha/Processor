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

enum STACKERRORS
{
    ACCESS_ERROR = 1<<0,
    SIZE_ERROR   = 1<<1,
    INFO_ERROR   = 1<<2,
    DATA_ERROR   = 1<<3,
    STRUCT_ERROR = 1<<4,
    STATUS_ERROR = 1<<5,
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

    double  stkPop      (double *arg);

    ~Processor ();

private:

    int ip = 0;
    int code_size = 0;

    stack<double> stk;
    stack<double> call_stk;

    vector<double> regs;
    char* code;

    double *ram;
};

size_t getfileSize (const string file_name);

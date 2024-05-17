#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include <cmath>
#include <stack>
#include <fstream>
#include <cassert>
#include <chrono>

/// @file Should be used as a reference handbook to processor (a.k.a. documentation)

#define SIGNATURE "CP01"

class Processor {

public:

    /// enum for register array, r0x is technical and not used in code
    enum Reg {

        r0x = 0,
        rax,
        rbx,
        rcx,
        rdx,
    };

    /// masks for command and argument bits
    enum Masks {

        MASK_RAM = (1<<7),
        MASK_REG = (1<<6),
        MASK_IMM = (1<<5),
        MASK_CMD = (1<<5) - 1
    };

    /// errors of some sort, stored as counters in array of ERR_TYPE_CNT size
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

    /**
     * @brief Reads code from input file to vector of code
     *
     * @param input_file_name name of input file
     */
    void    readCode        (const std::string input_file_name);

    /**
     * @brief checks for signature correctness
     *
     */
    void    checkSign       ();

    /**
     * @brief processes command argument and returns ptr to argument value/where to put value for argument (depends on command)
     *
     * @param cmd cmd byte to determine arg type
     * @return int* ptr to argument
     */
    int*    getArg          (char cmd);

    /**
     * @brief runs processor and executes code
     *
     */
    void    runCpu          ();

    /**
     * @brief gets ifstream size
     *
     * @param input_file ifstream ref to measure
     * @return size_t size (0 if something went wrong)
     */
    size_t  getfileSize     (std::ifstream& input_file);

    /**
     * @brief checks for errors
     *
     * @return true
     * @return false
     */
    bool    thereAreErrors   ();

    int     stkPop          (); ///< pops stack
    int     stkCallPop      (); ///< pops call stack
    void    stkPush         (int arg); ///< pushes arg to stack
    void    stkCallPush     (int arg); ///< pushes arg to call stack

    ~Processor ();

private:

    static const int CODE_SHIFT = 4; ///< signature size a.k.a. amount to shift code reader to read real code
    static const int REG_SIZE = 5; ///< amount of registers
    static const int RAM_SIZE = 121; ///< size of ram
    static const size_t STACK_CAPACITY = 16; ///< stk capacity
    static const size_t CALL_STACK_CAPACITY = 8; ///< call stack cap

    /**
     * @brief enum that creates constants of type CMD_push = 1 (for example)
     *
     */
    enum def_commands {

        #define DEF_CMD(name, num, arg, ...) \
            CMD_##name = num,\

        #include "../lib/cmd.hpp"

        #undef DEF_CMD
    };

    int ip = 0; ///< current instruction number in code
    int code_size = 0; ///< total size of code

    /// Мб либо свапнуть стек на пару статик массив + указатель на верх стека, либо убрать лимит по размеру
    /// пока второе
    std::stack<int> stk; ///< stack for operation
    std::stack<int> call_stk; ///< call stack for operation
    std::vector<char> code; ///< code array (binary)

    int regs[REG_SIZE] = {0}; ///< registers array
    int errors[ERR_TYPE_CNT] = {0}; ///< error arrays
    int ram[RAM_SIZE] = {0}; ///< ram array
};

#define ERRORCHECK if (thereAreErrors()) return;

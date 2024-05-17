#include "asm.hpp"

int main (int argc, char* argv[]) {

    /// Create assembler and error code variables
    Asm assembler;
    Asm::ErrorCode errorCode;

    errorCode = assembler.processComline (argc, argv); /// process com line args
    if (errorCode != Asm::ErrorCode::OK) return -1; ///stop if bad input

    errorCode = assembler.assembleBin (); /// basically do all the work
    if (errorCode != Asm::ErrorCode::OK) {
        std::cout << "killed in action\n\n" << (int) errorCode; /// if errors encountered stop and print error number
        return (int) errorCode;
    }

    return 0; /// 0 if all ok
}
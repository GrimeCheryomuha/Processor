#include "asm.hpp"

int main (int argc, char* argv[]) {

    Asm assembler;
    Asm::ErrorCode errorCode;

    errorCode = assembler.processComline (argc, argv);
    if (errorCode != Asm::ErrorCode::OK) return -1;

    errorCode = assembler.assembleBin ();
    if (errorCode != Asm::ErrorCode::OK) {
        std::cout << "killed in action\n\n" << (int) errorCode;
        return (int) errorCode;
    }

    return 0;
}
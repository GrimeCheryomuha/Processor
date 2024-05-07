#include "asm.hpp"

Asm::Asm () {

    #define DEF_CMD(name, num, arg_t, code) cmdTable.emplace (#name, num);

    #include "../cmd.hpp"

    #undef DEF_CMD
}

Asm::ErrorCode Asm::AssembleBin () {

    std::ifstream inputStream (inputFileName);
    
}

Asm::ErrorCode Asm::ProcessComLine (int argc, char* argv[]) {

    switch (argc) {

        case 2:

            if (argv[1][0] == '-') {

                std::cout << "No input file specified, exiting" << std::endl;
                return BAD_COM_LINE;
            }

            std::cout << "No output file specified, using default " << DEFAULT_BIN_FILE_NAME << ";" << std::endl;
            strcpy (inputFileName, argv[1]);
        break;

        case 4:

            if (argv[1][0] == '-') {

                std::cout << "No input file specified, exiting" << std::endl;
                return BAD_COM_LINE;
            }

            strcpy (inputFileName, argv[1]);

            if (strcmp (argv[2], "-o")) {

                std::cout << "Bad com line args" << std::endl;
                return BAD_COM_LINE;
            }

            if (argv[3][0] == '-') {

                std::cout << "Bad com line args" << std::endl;
                return BAD_COM_LINE;
            }

            strcpy (outputFileName, argv[3]);

        break;

        default:

            std::cout << "Bad com line args" << std::endl;
        return BAD_COM_LINE;
    }

    return OK;
}
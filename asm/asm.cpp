#include "asm.hpp"

Asm::Asm () {

    #define DEF_CMD(name, num, hasArg, ...) cmdTable.emplace (#name, std::pair<CmdNum_t, HasArg_t> (num, hasArg));

    #include "../lib/cmd.hpp"

    #undef DEF_CMD
}

Asm::ErrorCode Asm::assembleBin () {

    ErrorCode retVal = ErrorCode::OK;

    retVal = bufferizeInput ();
    if (retVal != ErrorCode::OK) return retVal;
    std::cout << "bufferized\n";

    retVal = writeSignatureAndClear ();
    if (retVal != ErrorCode::OK) return retVal;
    std::cout << "written sign\n";

    retVal = firstPass ();
    if (retVal != ErrorCode::OK) return retVal;
    std::cout << "first pass\n";

    retVal = secondPass ();
    if (retVal != ErrorCode::OK) return retVal;
    std::cout << "second pass\n";

    retVal = writeToFile ();
    std::cout << "written to file\n";
    return retVal;
}

Asm::ErrorCode Asm::writeToFile () {

    std::ofstream outputFile;

    outputFile.open (outputFileName);
    outputFile.clear ();

    outputFile << binOutputBuffer;

    outputFile.close ();

    return ErrorCode::OK;
}

Asm::ErrorCode Asm::secondPass () {

    int outputIndex = 4;

    for (int i = 0; i < inputBuffer.size (); i++) {

        if (cmdTable[inputBuffer[i]].second) {

            i++;
            outputIndex++;
            if (tagTable.count (inputBuffer[i])) {

                int arg = tagTable[inputBuffer[i]];
                char argCharArray[] = {*((char*)&arg), *((char*)&arg + 1), *((char*)&arg + 2), *((char*)&arg + 3)};
                binOutputBuffer[outputIndex++] = argCharArray[0];
                binOutputBuffer[outputIndex++] = argCharArray[1];
                binOutputBuffer[outputIndex++] = argCharArray[2];
                binOutputBuffer[outputIndex++] = argCharArray[3];
            }
            else outputIndex += 4;
        }
    }

    // if (outputIndex != binOutputBuffer.size ()) return ErrorCode::GENERIC_ERROR;
    return ErrorCode::OK;
}

Asm::ErrorCode Asm::firstPass () {

    ErrorCode retVal = ErrorCode::OK;

    for (int i = 0; i < inputBuffer.size (); i++) {

        if (cmdTable.count (inputBuffer[i]) != 0) {

            binOutputBuffer.push_back (cmdTable[inputBuffer[i]].first);

            if (cmdTable[inputBuffer[i]].second == false) continue;

            std::cout << "about to parse some args\n";

            retVal = parseArg (++i, binOutputBuffer.back ());
            if (retVal != ErrorCode::OK) return retVal;
        }
        else if (inputBuffer[i].back () == ':') {

            tagTable.emplace (inputBuffer[i], binOutputBuffer.size ());
        }
        else return ErrorCode::UNKNOWN_COMMAND;
    }

    return retVal;
}

/// @warning may be buggy
Asm::ErrorCode Asm::parseArg (int index, char& cmdByte) {

    if (inputBuffer[index][0] >= '0' and inputBuffer[index][0] <= '9' or inputBuffer[index][0] == '-') {

        cmdByte |= (char) Masks::MASK_IMM;

        int arg = std::stoi (inputBuffer[index]);
        char argCharArray[] = {*((char*)&arg), *((char*)&arg + 1), *((char*)&arg + 2), *((char*)&arg + 3)};
        binOutputBuffer.append (argCharArray);
    }
    else if (inputBuffer[index][0] == 'r' and inputBuffer[index].back () == 'x' and inputBuffer.size () > 2) {

        cmdByte |= (char) Masks::MASK_REG;

        binOutputBuffer.push_back (inputBuffer[index][1] == '0' ? 0 : inputBuffer[index][1] - 'a');
    }
    else if (inputBuffer[index][0] == '[' and inputBuffer[index].back () == ']') {

        cmdByte |= (char) Masks::MASK_RAM;

        if (inputBuffer[index][1] >= '0' and inputBuffer[index][0] <= '9' or inputBuffer[index][0] == '-') {

            cmdByte |= (char) Masks::MASK_IMM;

            int arg = std::stoi (inputBuffer[index]);
            char argCharArray[] = {*((char*)&arg), *((char*)&arg + 1), *((char*)&arg + 2), *((char*)&arg + 3)};
            binOutputBuffer.append (argCharArray);
        }
        else if (inputBuffer[index][1] == 'r' and inputBuffer[index][inputBuffer.size () - 2] == 'x') {

            cmdByte |= (char) Masks::MASK_REG;

            binOutputBuffer.push_back (inputBuffer[index][1] == '0' ? 0 : inputBuffer[index][1] - 'a');
        }
    }
    else if (tagTable.count (inputBuffer[index])) {

        int arg = tagTable[inputBuffer[index]];
        char argCharArray[] = {*((char*)&arg), *((char*)&arg + 1), *((char*)&arg + 2), *((char*)&arg + 3)};
        binOutputBuffer.append (argCharArray);
    }
    else {

        int arg = -1;
        char argCharArray[] = {*((char*)&arg), *((char*)&arg + 1), *((char*)&arg + 2), *((char*)&arg + 3)};
        binOutputBuffer.append (argCharArray);
    }

    return ErrorCode::OK;
}

Asm::ErrorCode Asm::writeSignatureAndClear () {

    try {
        binOutputBuffer.append (SIGNATURE);
    }
    catch (...) {

        return ErrorCode::BAD_OUTPUT_BUFFER;
    }

    return ErrorCode::OK;
}

Asm::ErrorCode Asm::bufferizeInput () {

    ErrorCode retVal = checkInputFileExt ();
    if (retVal != ErrorCode::OK) return retVal;

    if (inputFileName[0] =='\0') return ErrorCode::BAD_INPUT_FILE;

    std::ifstream inputFile;
    try {

        inputFile.open (inputFileName);
    }
    catch (...) {

        return ErrorCode::BAD_INPUT_FILE;
    }

    char temp[256] = "";

    while (inputFile >> temp) inputBuffer.emplace_back (temp);

    inputFile.close ();

    return ErrorCode::OK;
}

Asm::ErrorCode Asm::checkInputFileExt () {

    if (strlen (inputFileName) <= strlen (CODE_FILE_EXT)) return ErrorCode::BAD_INPUT_FILE_NAME;
    if (strcmp (inputFileName + strlen (inputFileName) - strlen (CODE_FILE_EXT), CODE_FILE_EXT)) return ErrorCode::BAD_INPUT_FILE_NAME;
    return ErrorCode::OK;
}

Asm::ErrorCode Asm::processComline (int argc, char* argv[]) {

    switch (argc) {

        case 2:

            if (argv[1][0] == '-') {

                std::cout << "No input file specified, exiting" << std::endl;
                return ErrorCode::BAD_COM_LINE;
            }

            std::cout << "No output file specified, using default " << DEFAULT_BIN_FILE_NAME << ";" << std::endl;
            strcpy (inputFileName, argv[1]);

            strcpy (outputFileName, DEFAULT_BIN_FILE_NAME);
        break;

        case 4:

            if (argv[1][0] == '-') {

                std::cout << "No input file specified, exiting" << std::endl;
                return ErrorCode::BAD_COM_LINE;
            }

            strcpy (inputFileName, argv[1]);

            if (strcmp (argv[2], "-o")) {

                std::cout << "Bad com line args" << std::endl;
                return ErrorCode::BAD_COM_LINE;
            }

            if (argv[3][0] == '-') {

                std::cout << "Bad com line args" << std::endl;
                return ErrorCode::BAD_COM_LINE;
            }

            strcpy (outputFileName, argv[3]);

        break;

        default:

            std::cout << "Bad com line args" << std::endl;
        return ErrorCode::BAD_COM_LINE;
    }

    return ErrorCode::OK;
}

Asm::~Asm () {};
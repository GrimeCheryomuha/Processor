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

    retVal = writeSignatureAndClear ();
    if (retVal != ErrorCode::OK) return retVal;

    retVal = firstPass ();
    if (retVal != ErrorCode::OK) return retVal;

    retVal = secondPass ();
    if (retVal != ErrorCode::OK) return retVal;

    retVal = writeToFile ();
    return retVal;
}

Asm::ErrorCode Asm::writeToFile () {

    std::ofstream outputFile;

    /// open + clear output file
    outputFile.open (outputFileName);
    outputFile.clear ();

    for (auto i : binOutputBuffer) {

        outputFile << i;
    }

    outputFile.close ();

    return ErrorCode::OK;
}

Asm::ErrorCode Asm::secondPass () {

    int outputIndex = sizeof (SIGNATURE) - 1;
    for (int i = 0; i < inputBuffer.size (); i++) {

        // uncomment for diagnostics
        // std::cout << "Current second pass parameters: outPutIndex = " << outputIndex << "; index = " << i << "; input buffer value = " << inputBuffer[i] << "; output value = ";
        // printf ("%X;\n", binOutputBuffer[outputIndex]);

        /// skips if encountered tag
        if (tagTable.count (inputBuffer[i])) continue;

        /// if command has arguments
        if (cmdTable[inputBuffer[i]].second) {

            i++;
            outputIndex++;

            /// if argument is a tag replace it with jump destination (ip)
            if (tagTable.count (inputBuffer[i])) {

                int arg = tagTable[inputBuffer[i]];
                binOutputBuffer[outputIndex + 0] = *((char*)&arg + 0);
                binOutputBuffer[outputIndex + 1] = *((char*)&arg + 1);
                binOutputBuffer[outputIndex + 2] = *((char*)&arg + 2);
                binOutputBuffer[outputIndex + 3] = *((char*)&arg + 3);
            }

            outputIndex += 3;/// +3 because +1 will be later
        }

        outputIndex++; ///... here
    }

    if (outputIndex != binOutputBuffer.size ()) return ErrorCode::GENERIC_ERROR; /// justa check that we didn't skip or not read bytes
    return ErrorCode::OK;
}

Asm::ErrorCode Asm::firstPass () {

    ErrorCode retVal = ErrorCode::OK;

    for (int i = 0; i < inputBuffer.size (); i++) {

        /// if is a command
        if (cmdTable.count (inputBuffer[i]) != 0) {

            binOutputBuffer.push_back (cmdTable[inputBuffer[i]].first); /// writes command byte

            if (cmdTable[inputBuffer[i]].second == false) continue; ///skips parseArg if no args

            retVal = parseArg (++i, binOutputBuffer.back ());///parses arg and moves index to argument string
            if (retVal != ErrorCode::OK) return retVal;
        }
        else if (inputBuffer[i].back () == ':') { /// if is a tag

            inputBuffer[i].pop_back (); ///removes ':' from the end
            tagTable.emplace (inputBuffer[i], binOutputBuffer.size ()); ///adds pair of tag name and adress to known tags
        }
        else return ErrorCode::UNKNOWN_COMMAND; /// if not a tag and not a command found
    }

    return retVal;
}

Asm::ErrorCode Asm::parseArg (int index, char& cmdByte) {

    /// @warning all args are int!

    /// if arg is a number (better check should be implemented later)
    if (inputBuffer[index][0] >= '0' and inputBuffer[index][0] <= '9' or inputBuffer[index][0] == '-' or inputBuffer[index][0] == '+') {

        cmdByte |= (char) Masks::MASK_IMM; ///adds bit for immidiate constant argument

        int arg = std::stoi (inputBuffer[index]);
        binOutputBuffer.push_back (*((char*)&arg + 0));
        binOutputBuffer.push_back (*((char*)&arg + 1));
        binOutputBuffer.push_back (*((char*)&arg + 2));
        binOutputBuffer.push_back (*((char*)&arg + 3));
    }
    else if (inputBuffer[index][0] == 'r' and inputBuffer[index].back () == 'x' and inputBuffer.size () > 2) { /// if argument is a register

        cmdByte |= (char) Masks::MASK_REG; /// adds bit for register argument

        binOutputBuffer.push_back (inputBuffer[index][1] == '0' ? 0 : inputBuffer[index][1] - 'a' + 1);
        binOutputBuffer.push_back (0);
        binOutputBuffer.push_back (0);
        binOutputBuffer.push_back (0);
    }
    else if (inputBuffer[index][0] == '[' and inputBuffer[index].back () == ']') { /// if argument is ram, than processor uses imm or reg argument as adress in ram

        cmdByte |= (char) Masks::MASK_RAM; /// adds ram bit

        if (inputBuffer[index][1] >= '0' and inputBuffer[index][1] <= '9' or inputBuffer[index][0] == '-') { /// same as imm

            cmdByte |= (char) Masks::MASK_IMM;

            int arg = std::stoi (inputBuffer[index]);
            binOutputBuffer.push_back (*((char*)&arg + 0));
            binOutputBuffer.push_back (*((char*)&arg + 1));
            binOutputBuffer.push_back (*((char*)&arg + 2));
            binOutputBuffer.push_back (*((char*)&arg + 3));
        }
        else if (inputBuffer[index][1] == 'r' and inputBuffer[index][inputBuffer[index].size () - 2] == 'x') { /// same as reg

            cmdByte |= (char) Masks::MASK_REG;

            binOutputBuffer.push_back (inputBuffer[index][2] == '0' ? 0 : inputBuffer[index][2] - 'a' + 1);
            binOutputBuffer.push_back (0);
            binOutputBuffer.push_back (0);
            binOutputBuffer.push_back (0);
        }
    }
    else if (tagTable.count (inputBuffer[index])) { /// if not ram reg nor imm and found in tag table, then write corresponding adress


        int arg = tagTable[inputBuffer[index]];
        binOutputBuffer.push_back (*((char*)&arg + 0));
        binOutputBuffer.push_back (*((char*)&arg + 1));
        binOutputBuffer.push_back (*((char*)&arg + 2));
        binOutputBuffer.push_back (*((char*)&arg + 3));
    }
    else { /// if not ram reg nor imm and not found in tag table, then consider it a yet unknown tag (if this tag does not exist than secondPass will fail)

        int arg = -1;
        binOutputBuffer.push_back (*((char*)&arg + 0));
        binOutputBuffer.push_back (*((char*)&arg + 1));
        binOutputBuffer.push_back (*((char*)&arg + 2));
        binOutputBuffer.push_back (*((char*)&arg + 3));
    }

    return ErrorCode::OK;
}

Asm::ErrorCode Asm::writeSignatureAndClear () {

    try {
        binOutputBuffer.push_back (SIGNATURE[0]);
        binOutputBuffer.push_back (SIGNATURE[1]);
        binOutputBuffer.push_back (SIGNATURE[2]);
        binOutputBuffer.push_back (SIGNATURE[3]);
    }
    catch (...) {

        return ErrorCode::BAD_OUTPUT_BUFFER;
    }

    return ErrorCode::OK;
}

Asm::ErrorCode Asm::bufferizeInput () {

    ErrorCode retVal = checkInputFileExt (); /// checks if extension matches
    if (retVal != ErrorCode::OK) return retVal;

    if (inputFileName[0] =='\0') return ErrorCode::BAD_INPUT_FILE; ///if name is non existent

    std::ifstream inputFile;
    try {

        inputFile.open (inputFileName);
    }
    catch (...) {

        return ErrorCode::BAD_INPUT_FILE;
    }

    char temp[256] = ""; /// temporary var for words

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
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cstring>

#define DEFAULT_BIN_FILE_NAME "a.grime_asm"
#define CODE_FILE_EXT ".grime_code"
#define SIGNATURE "CP01"


class Asm {

    public:

    typedef char CmdNum_t;
    typedef bool HasArg_t;

    enum class ErrorCode {

        OK              = 0,
        GENERIC_ERROR,
        BAD_COM_LINE,
        BAD_INPUT_FILE,
        BAD_OUTPUT_BUFFER,
        UNKNOWN_COMMAND,
        LOST_TAG_JMP,
        BAD_INPUT_FILE_NAME,
    };

    enum class Masks {

        MASK_RAM = 1<<7,      ///< Mask for ram access
        MASK_REG = 1<<6,      ///< Mask for reg access
        MASK_IMM = 1<<5,      ///< Mask for immidiate constant
        MASK_CMD = (1<<5) - 1 ///< Mask for actual command number
    };

    Asm ();

    ErrorCode   processComline      (int argc, char* argv[]);
    ErrorCode   assembleBin         ();

    ~Asm ();

    private:

    ErrorCode   bufferizeInput              ();
    ErrorCode   checkInputFileExt           ();
    ErrorCode   writeOutput                 ();
    ErrorCode   writeSignatureAndClear      ();
    ErrorCode   parseArg                    (int index, char& cmdByte);
    ErrorCode   firstPass                   ();
    ErrorCode   secondPass                  ();
    ErrorCode   writeToFile                 ();

    char        inputFileName[256] = "";
    char        outputFileName[256] = "";
    std::vector<std::string> inputBuffer;
    std::string binOutputBuffer;

    std::unordered_map<std::string, unsigned int> tagTable;
    std::unordered_map<std::string, std::pair<CmdNum_t, HasArg_t>> cmdTable;
};
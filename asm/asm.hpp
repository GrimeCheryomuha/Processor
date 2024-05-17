#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cstring>

/// default output file name
#define DEFAULT_BIN_FILE_NAME "a.grime_asm"
/// a proper extension for input file
#define CODE_FILE_EXT ".grime_code"
/// a signature that should display file type and version, but is not yet properly implemented
#define SIGNATURE "CP01"

/// @file this is a header for assembler part of this project. It contains a class, couple defines and constants. This should be referred to as project documentation i.e. to check what different functions do or what variables stand for
/// @file it is because most comments will be written here

/**
 * @brief an assembler class that is used to convert .grime_code to .grime_asm
 * 
 */
class Asm {

    public:

    typedef char CmdNum_t; ///< basically to show that it's not just a char
    typedef bool HasArg_t; ///< same as previous

    /// @brief enum for error codes. Not meant to be stored as multiple codes in one variable
    enum class ErrorCode {

        OK              = 0,
        GENERIC_ERROR,          ///< use this if u have no time to think of specific error
        BAD_COM_LINE,           ///< command line arguments are wrong
        BAD_INPUT_FILE,         ///< if something wrong with input file
        BAD_OUTPUT_BUFFER,      ///< returned if some error occured with writing to output buffer
        UNKNOWN_COMMAND,        ///< if unknown command ecnountered in secondPass
        LOST_TAG_JMP,           ///< currently unused, but should display that jump refers to non existing tag
        BAD_INPUT_FILE_NAME,    ///< returned if input file has wrong extension, should be CODE_FILE_EXT
    };

    enum class Masks {

        MASK_RAM = 1<<7,      ///< Mask for ram access
        MASK_REG = 1<<6,      ///< Mask for reg access
        MASK_IMM = 1<<5,      ///< Mask for immidiate constant
        MASK_CMD = (1<<5) - 1 ///< Mask for actual command number
    };

    /**
     * @brief Construct a new Asm object and fill cmdTable with DEF_CMD from ../lib/cmd.hpp
     */
    Asm ();

    /**
     * @brief Processes com line args
     *
     * @param argc argc from main
     * @param argv argv from main
     * @return ErrorCode if something wrong, OK otherwise
     */
    ErrorCode   processComline      (int argc, char* argv[]);

    /**
     * @brief a funciton that basically does all the work
     *
     * @return ErrorCode errors that occur in sub functions (defined in private part)
     */
    ErrorCode   assembleBin         ();

    /**
     * @brief Destroy the Asm object
     *
     */
    ~Asm ();

    private:

    /**
     * @brief bufferizes input file to inputBuffer so that it's faster
     *
     * @return ErrorCode
     */
    ErrorCode   bufferizeInput              ();

    /**
     * @brief checks if input file extension is right
     *
     * @return ErrorCode
     */
    ErrorCode   checkInputFileExt           ();

    /**
     * @brief writes binary part (is a hub function of some sorts)
     *
     * @return ErrorCode
     */
    ErrorCode   writeOutput                 ();

    /**
     * @brief clears output buffer just in case and writes SIGNATURE
     *
     * @return ErrorCode
     */
    ErrorCode   writeSignatureAndClear      ();

    /**
     * @brief processes arguments for command
     *
     * @param index index that points to argument string
     * @param cmdByte command byte so that argument type could be deduced
     * @return ErrorCode
     */
    ErrorCode   parseArg                    (int index, char& cmdByte);

    /**
     * @brief first pass that converts all commands and arguments to binary, stores all tags and writes them where it can. Plugs with FFFFFFFF when tag is unknown on moment of processing jump command
     *
     * @return ErrorCode
     */
    ErrorCode   firstPass                   ();

    /**
     * @brief seconf pass that replaces all FFFFFFFF plugs with real ips of jump destinations (that were stored in tagTable during first pass)
     *
     * @return ErrorCode
     */
    ErrorCode   secondPass                  ();

    /**
     * @brief just writes whole of binOuyputBuffer to outputfile
     *
     * @return ErrorCode
     */
    ErrorCode   writeToFile                 ();

    char                        inputFileName[256] = "";
    char                        outputFileName[256] = "";
    std::vector<std::string>    inputBuffer;
    std::vector<char>           binOutputBuffer;

    std::unordered_map<std::string, unsigned int> tagTable; ///< a map that stores tag name and tag location
    std::unordered_map<std::string, std::pair<CmdNum_t, HasArg_t>> cmdTable; ///< a map for command names and byte+has_arg pair
};
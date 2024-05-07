#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cstring>

#define DEFAULT_BIN_FILE_NAME "a.grime_asm"

class Asm {

    public:

    typedef unsigned int Hash_t;

    enum ErrorCode {

        OK              = 0,
        BAD_COM_LINE,

    };

    Asm ();

    ErrorCode   ProcessComLine      (int argc, char* argv[]);
    ErrorCode   AssembleBin         ();
    ErrorCode   WriteOutput         (const char* filename);

    ~Asm ();

    private:

    char        inputFileName[256];
    char        outputFileName[256];
    std::string textInputBuffer;
    std::string binOutputBuffer;

    std::unordered_map<std::string, unsigned int> tagTable;
    std::unordered_map<std::string, std::pair<char, char>> cmdTable;
};
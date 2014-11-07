#include <stdexcept>
#include <fstream>
#include "common_operations.h"

bool fileComparison(const std::string& lFilePath, const std::string& rFilePath)
{
    int BUFFER_SIZE = 1000;

    std::ifstream lFile(lFilePath.c_str(), std::ifstream::in | std::ifstream::binary);
    std::ifstream rFile(rFilePath.c_str(), std::ifstream::in | std::ifstream::binary);

    if(!lFile.is_open() || !rFile.is_open())
    {
        return false;
    }

    char *lBuffer = new char[BUFFER_SIZE]();
    char *rBuffer = new char[BUFFER_SIZE]();

    do {
        lFile.read(lBuffer, BUFFER_SIZE);
        rFile.read(rBuffer, BUFFER_SIZE);

        if (memcmp(lBuffer, rBuffer, BUFFER_SIZE) != 0)
        {
            delete[] lBuffer;
            delete[] rBuffer;
            return false;
        }
    } while (lFile.good() || rFile.good());

    delete[] lBuffer;
    delete[] rBuffer;
    return true;
}


inline bool fileExist(const std::string& name)
{
    std::ifstream file(name);
    if(!file)    //if the file was not found, then file is 0, i.e. !file=1 or true
        return false;    //the file was not found
    else         //if the file was found, then file is non-0
        return true;     //the file was found
}

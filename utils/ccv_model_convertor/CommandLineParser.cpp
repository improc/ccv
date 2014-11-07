#include "CommandLineParser.h"


CommandLineParser::CommandLineParser(int argc, const char* argv[])
{
    int argIdx = 1;
    while( argIdx < argc )
    {   
        string key = "";
        char keyChar;
        while( argIdx < argc ) {
            key = string(argv[argIdx]);

            if (CommandLineParser::isKey(key) == true)
            {
                keyChar = key[1];
                this->pairs[keyChar] = "";
                argIdx++;
            }
            else
            {
                break;
            }
        };

        if ( argIdx < argc )
        {
            string val = argv[argIdx++];
            this->pairs[keyChar] = val;
        }
    }
}


CommandLineParser::~CommandLineParser(void)
{
}
bool CommandLineParser::containsValue(char key)
{
    return this->pairs.find(key) != this->pairs.end();
}

string CommandLineParser::getValue(char key)
{
    return this->pairs[key];
}
int CommandLineParser::getIntValue(char key)
{
    return atoi(this->pairs[key].c_str());
}

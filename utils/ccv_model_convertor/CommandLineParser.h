#pragma once

#include <stdio.h>
#include <map>
#include <string>

using namespace std;

class CommandLineParser
{
    map<char, string> pairs;

public:
    CommandLineParser(int argc, const char* argv[]);
    ~CommandLineParser(void);

    bool containsValue(char key);

    int getIntValue(char key);
    string getValue(char key);

private:
    static bool isKey(string key) {return (strlen(key.c_str()) == 2U && key[0] == '-');}
};


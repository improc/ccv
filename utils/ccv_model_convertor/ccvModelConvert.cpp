#include <stdio.h>

extern "C" {
#include "ccv.h"
}

#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>

#include "CommandLineParser.h"

using namespace std;

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

int main(int argc, const char* argv[])
{
    CommandLineParser clp(argc, argv);
    string inputFile = clp.getValue('s');
    
    if (true == clp.containsValue('h') || 
        false == clp.containsValue('i') )
    {
        printf("please specify input and output - example usage: ccvModelConvertor.exe -i input_filename -o output_filename [if no, then automatically] -m mode [0=text file, 1=header]");
        return 1;
    }

    int mode = (clp.containsValue('m'))?clp.getIntValue('m'):0;

    string modelName = clp.getValue('i');
    ccv_convnet_t* model = ccv_convnet_read(0, modelName.c_str());
    if (model == nullptr)
    {
        printf("model not loaded properly, please check path %s\n", modelName.c_str());
        return 0;
    }
    else
    {
        printf("model loaded properly\n");
    }

    ccv_convnet_write_param_t outParams;
    outParams.half_precision = true;

    string outName = clp.getValue('o');
    if (0 == strlen(outName.c_str())) 
    {
        outName = string(modelName).append("_conv");
        if (mode == 1) 
        {
            outName.append(".h");
        }
        else
        {
            outName.append(".txt");
        }
    }

    printf("out name: %s\n", outName.c_str());

    ccv_convnet_write_file(model, outName.c_str(), outParams);

    if (mode == 1)
    {
        std::fstream is;
        is.open (outName, std::fstream::in | std::fstream::out | std::fstream::app);
        string wholeStr;
        string line;

        while ( is.good() )
        {
            getline (is,line);
            wholeStr.append(line);
        }

        is.close();

        std::fstream os;
        os.open (outName, std::fstream::out);
        
        int wholeLen = wholeStr.length();
        int divider = 100;

        string defineName = outName;
        replaceAll(defineName, ".", "_");
        std::transform(defineName.begin(), defineName.end(),defineName.begin(), ::toupper);
        
        string variableName = defineName;
        std::transform(variableName.begin(), variableName.end(),variableName.begin(), ::tolower);

        // os << "#ifndef " << defineName << endl;
        // os << "#define " << defineName << endl;
        os << "#pragma once " << endl;

        os << "#include <string.h>" << endl;
        os << "using namespace std;" << endl << endl;

        os << "int model_num_" << variableName << " = " << ceil(wholeLen/(float)divider) << ";" << endl;
        os << "char* model_" << variableName << "[" << ceil(wholeLen/(float)divider) << "] = {" << endl;

        for (int i=0; i<wholeLen; i+=divider)
        {
            int len = i+divider < wholeLen? divider : wholeLen - i;
            os << "\"" << wholeStr.substr(i, len) << "\"," << endl;
        }
        os << "};" << endl;

        // os << "#endif // " << defineName << endl;

        os.close();
    }

    printf ("done");
    return 0;
}
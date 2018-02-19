#ifndef CLML_UTILS_HEADER_
#define CLML_UTILS_HEADER_

#include <string>
#include <list>
#include <fstream>

class ClmtUtils{
public:
    static int toInteger(std::string s);
    static void makeNumberValid(int& number, int low, int high, bool inclusive);
    static std::list<int> retrieveNumbers(std::string inputString, int totalNumber);
    static int getNumLines(std::string fileName);
};

#endif
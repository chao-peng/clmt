#ifndef CLML_UTILS_HEADER_
#define CLML_UTILS_HEADER_

#include <string>
#include <list>
#include <fstream>
#include <map>

class ClmtUtils{
public:
    static int toInteger(std::string s);
    static void makeNumberValid(int& number, int low, int high, bool inclusive);
    static std::list<int> retrieveNumbers(std::string inputString, int totalNumber);
    static int getNumLines(std::string fileName);
    static bool hasQuality(const unsigned int& quality, const unsigned int& qualityIdentifier);
    static void initialiseOperatorTypeMap(std::map<std::string, unsigned int>& operatorTypeMap);
    static void initialiseMutantOperatorMap(std::map<std::string, std::list<std::string>>& mutantOperatorMap);
    static std::string colorString(const std::string& str, const char* const color);
    static void generateMutant(const std::string& kernelFilename, const int& numMutableOperators);
};

#endif
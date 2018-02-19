#include "Utils.h"
#include "Constants.h"

#include <iostream>
#include <list>
#include <set>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>

int ClmtUtils::toInteger(std::string s){
    int power = 1;
    int result = 0;
    char c;
    for (int i = s.length() - 1; i >= 0; --i){
        c = s.at(i);
        if (!isdigit(c)) return 0;
        result = result + power * (c - '0');
        power *= 10;
    }
    return result;
}

void ClmtUtils::makeNumberValid(int& number, int low, int high, bool inclusive = true){
    if (!inclusive){
        low = low + 1;
        high = high - 1;
    }
    if (number < low) number = low;
    if (number > high) number = high;
}

std::list<int> ClmtUtils::retrieveNumbers(std::string inputString, int totalNumber){
    inputString = inputString + ',';
    std::list<int> numberList;
    std::string part = "";
    size_t i = 0;
    bool expectingSecondNumber = false;
    bool isNegative = false;
    int first, second;
    char c;
    while(i < inputString.length()){
        c = inputString.at(i);

        if (c == '-'){
            isNegative = true;
            ++i;
            continue;
        } else if (c == ':'){
            if (part.empty()){
                ++i;
                continue;
            }
            first = toInteger(part);
            if (isNegative) {
                first = totalNumber - first + 1;
                isNegative = false;
            }
                makeNumberValid(first, 1, totalNumber);
                expectingSecondNumber = true;
                part = "";
        } else if (c == ','){
            if (part.empty()){
                ++i;
                continue;
            }
            if (expectingSecondNumber){
                expectingSecondNumber = false;
                second = toInteger(part);
                if (isNegative) {
                    second = totalNumber - second + 1;
                    isNegative = false;
                }
                makeNumberValid(first, 1, totalNumber);
                makeNumberValid(second, 1, totalNumber);
                part = "";
                if (first == second) {
                    numberList.push_back(first);
                } else {
                    if (first > second) { //swap without introducing a new variable;
                        first = first + second;
                        second = first - second;
                        first = first - second;
                    }
            
                    while (first <= second){
                        numberList.push_back(first);
                        ++first;
                    }
                }
            } else {
                first = toInteger(part);
                if (isNegative) {
                    first = totalNumber - first + 1;
                    isNegative = false;
                }
                makeNumberValid(first, 1, totalNumber);
                numberList.push_back(first);
                part = "";
            }
        } else if (isdigit(c)){
            part = part + c;
        }
        ++i;
    }
    numberList.sort();
    numberList.unique();
    return numberList;
}

int ClmtUtils::getNumLines(std::string fileName){
    std::ifstream inFile(fileName);
    int i = std::count(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>(), '\n');
    inFile.close();
    return i;
}

bool ClmtUtils::hasQuality(const unsigned int& quality, const unsigned int& qualityIdentifier){
    return ((quality & qualityIdentifier) == qualityIdentifier)? true: false;
}

void ClmtUtils::initialiseOperatorTypeMap(std::map<std::string, unsigned int>& operatorTypeMap){
    operatorTypeMap["+B"]  = operator_type::ARITHMETIC;
    operatorTypeMap["-B"]  = operator_type::ARITHMETIC;
    operatorTypeMap["-U"]  = operator_type::ARITHMETIC;
    operatorTypeMap["*B"]   = operator_type::ARITHMETIC;
    operatorTypeMap["/B"]   = operator_type::ARITHMETIC;
    operatorTypeMap["%B"]   = operator_type::ARITHMETIC;
    operatorTypeMap["++U"]  = operator_type::ARITHMETIC;
    operatorTypeMap["--U"]  = operator_type::ARITHMETIC;
    operatorTypeMap["==B"]  = operator_type::RELATIONAL;
    operatorTypeMap["!=B"]  = operator_type::RELATIONAL;
    operatorTypeMap["<B"]   = operator_type::RELATIONAL;
    operatorTypeMap["<=B"]  = operator_type::RELATIONAL;
    operatorTypeMap[">B"]   = operator_type::RELATIONAL;
    operatorTypeMap[">=B"]  = operator_type::RELATIONAL;
    operatorTypeMap["&&B"]  = operator_type::LOGICAL;
    operatorTypeMap["||B"]  = operator_type::LOGICAL;
    operatorTypeMap["!U"]   = operator_type::LOGICAL;
    operatorTypeMap["&B"]   = operator_type::BITWISE;
    operatorTypeMap["|B"]   = operator_type::BITWISE;
    operatorTypeMap["^B"]   = operator_type::BITWISE;
    operatorTypeMap["~U"]   = operator_type::BITWISE;
    operatorTypeMap["<<B"]  = operator_type::BITWISE;
    operatorTypeMap[">>B"]  = operator_type::BITWISE;
    operatorTypeMap["=B"]   = operator_type::ASSIGNMENT;
    operatorTypeMap["+=B"]  = operator_type::ASSIGNMENT;
    operatorTypeMap["-=B"]  = operator_type::ASSIGNMENT;
    operatorTypeMap["*=B"]  = operator_type::ASSIGNMENT;
    operatorTypeMap["/=B"]  = operator_type::ASSIGNMENT;
    operatorTypeMap["%=B"]  = operator_type::ASSIGNMENT;
    operatorTypeMap["<<=B"] = operator_type::ASSIGNMENT;
    operatorTypeMap[">>=B"] = operator_type::ASSIGNMENT;
    operatorTypeMap["&=B"]  = operator_type::ASSIGNMENT;
    operatorTypeMap["|=B"]  = operator_type::ASSIGNMENT;
    operatorTypeMap["^=B"]  = operator_type::ASSIGNMENT;
    operatorTypeMap["&U"]   = operator_type::OTHER;
    operatorTypeMap["*U"]   = operator_type::OTHER;
}

void ClmtUtils::initialiseMutantOperatorMap(std::map<std::string, std::list<std::string>>& mutantOperatorMap){
    //Arithmetic
    mutantOperatorMap["+B"] = {"-"};
    mutantOperatorMap["-B"] = {"+"};
    mutantOperatorMap["-U"] = {""};
    mutantOperatorMap["*B"] = {"/"};
    mutantOperatorMap["/B"] = {"*"};
    mutantOperatorMap["%B"] = {"*"};
    mutantOperatorMap["++U"] = {"--"};
    mutantOperatorMap["--U"] = {"++"};
    //Relational
    mutantOperatorMap["<B"] = {">", ">="};
    mutantOperatorMap[">B"] = {"<", "<="};
    mutantOperatorMap["==B"] = {"!="};
    mutantOperatorMap["<=B"] = {">=", ">"};
    mutantOperatorMap[">=B"] = {"<=", "<"};
    mutantOperatorMap["!=B"] = {"=="};
    //Logical
    mutantOperatorMap["&&B"] = {"||"};
    mutantOperatorMap["||B"] = {"&&"};
    mutantOperatorMap["!U"] = {""};
    //Bitwise
    mutantOperatorMap["&B"] = {"|"};
    mutantOperatorMap["|B"] = {"&"};
    mutantOperatorMap["^B"] = {"&"};
    mutantOperatorMap["~U"] = {""};
    mutantOperatorMap["<<"] = {">>"};
    mutantOperatorMap[">>"] = {"<<"};
    //Assignment
    mutantOperatorMap["+=B"] = {"-="};
    mutantOperatorMap["-=B"] = {"+="};
    mutantOperatorMap["*=B"] = {"/="};
    mutantOperatorMap["/=B"] = {"*="};
    mutantOperatorMap["%=B"] = {"*="};
    mutantOperatorMap["<<=B"] = {">>="};
    mutantOperatorMap[">>=B"] = {"<<="};
    mutantOperatorMap["&=B"] = {"|="};
    mutantOperatorMap["|=B"] = {"&="};
    mutantOperatorMap["^=B"] = {"&="};
}

std::string ClmtUtils::colorString(const std::string& str, const char* const color){
    return color + str + output_color::KNRM;
}

void ClmtUtils::generateMutant(const std::string& kernelFilename, const int& numMutableOperators){
    std::ifstream kernelFile(kernelFilename);
    std::stringstream codeStream;
    std::string code;
    codeStream << kernelFile.rdbuf();
    code = codeStream.str();
    int i = numMutableOperators;
    while (i--){
        
    }
}

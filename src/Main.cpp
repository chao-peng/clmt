#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>

#include "llvm/Support/CommandLine.h"
#include "clang/Tooling/CommonOptionsParser.h"

static llvm::cl::OptionCategory ToolCategory("OpenCL kernel branch coverage checker options");

static llvm::cl::opt<std::string> executableFile(
    "run",
    llvm::cl::desc("Specify the executable file"),
    llvm::cl::value_desc("executable"),
    llvm::cl::Required
);

static llvm::cl::opt<std::string> expectedOutput(
    "compare",
    llvm::cl::desc("Specify the expected output file path"),
    llvm::cl::value_desc("filename"),
    llvm::cl::Required
);

static llvm::cl::opt<bool> completeMatch(
    "all",
    llvm::cl::desc("Compare output with the expected output file"),
    llvm::cl::value_desc("boolean"),
    llvm::cl::Optional
);

static llvm::cl::opt<std::string> matchLines(
    "lines",
    llvm::cl::desc("Compare certain lines of output with the expected output file"),
    llvm::cl::value_desc("number of lines"),
    llvm::cl::Optional
);

int main(int argc, const char** argv){
    clang::tooling::CommonOptionsParser optionsParser(argc, argv, ToolCategory);

    std::cout << "Kernel files:\n";
    for (auto it = optionsParser.getSourcePathList().begin(); it!=optionsParser.getSourcePathList().end();it++){
        std::string kernelFileName(it->c_str());
        std::cout << kernelFileName << std::endl;
    }

    std::cout << "Expected output: " << expectedOutput.c_str() << "\n";
    std::cout << "Executable: " << executableFile.c_str() << "\n";

    if (completeMatch && !matchLines.empty()){
        std::cout << "The matching mode can either be all or part match\n";
        exit(-1);
    } 

    if (!matchLines.empty()){
        std::cout << "Complete matching disabled\n";
        std::cout << matchLines.c_str() << "\n";
    } else {
        std::cout << "Complete matching enabled\n";
    }

}
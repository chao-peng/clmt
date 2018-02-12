#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <list>

#include "Utils.h"
#include "Constants.h"

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

    // Set of kernel files
    std::list<std::string> kernels;
    // Expected output file
    std::string expectedOutputFilePath;
    // Executable file
    std::string executableFilePath;
    // Lines in the expected output file to match
    std::list<int> lines;

    std::string kernelFileName;
    for (auto it = optionsParser.getSourcePathList().begin(); it!=optionsParser.getSourcePathList().end();it++){
        kernelFileName = it->c_str();
        kernels.push_back(kernelFileName);
    }

    if (kernels.size() == 0){
        std::cout << "Please provide at least one GPU kernel source code to process.\n";
        exit(error_code::KERNEL_FILE_NOT_PROVIDED);
    }

    expectedOutputFilePath = expectedOutput.c_str();
    int totalLines = ClmtUtils::getNumLines(expectedOutputFilePath);

    executableFilePath = executableFile.c_str();

    if (completeMatch && !matchLines.empty()){
        std::cout << "The matching mode should either be all (-all) or part match (specify lines in the format of \"3:5,7,-1\")\n";
        exit(error_code::LINES_TO_COMPARE_NOT_VALID);
    } 

    if (!matchLines.empty()){
        lines = ClmtUtils::retrieveNumbers(matchLines.c_str(), totalLines);
    } else {
        for (int i = 1; i <= totalLines; ++i){
            lines.push_back(i);
        }
    }

    int currentKernel = 0;
    for (auto itKernel = kernels.begin(); itKernel != kernels.end(); itKernel++){
        std::cout << "Processing kernel file " << *itKernel << " [" << currentKernel+1 << "/" << kernels.size() << "]" << std::endl;
        //TODO: implement kernel processing.
        //TODO: Print how many mutants generated.
        // e.g. 
        // 10 mutants generated for kernel1
        // 20 mutants generated for kernel2
        ++currentKernel;
    }

    std::cout << "Executing " << executableFilePath << std::endl;
    //TODO: Iterate every mutant and compare result
    // e.g.
    // Executing with mutant 1
    // killed / survived

    //TODO: Summary
    // e.g.
    // X mutants survived
    // Y mutants killed
    std::cout << "[Result]" << std::endl; 

}
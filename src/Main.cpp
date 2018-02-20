#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <list>
#include <map>

#include "Utils.h"
#include "Constants.h"
#include "OpenCLKernelInvastigator.h"

#include "llvm/Support/CommandLine.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

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

static llvm::cl::opt<std::string> userConfigFileName(
    "config",
    llvm::cl::desc("Specify the user config file name"),
    llvm::cl::value_desc("filename"),
    llvm::cl::Optional // Will be empty string if not specified
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

    std::list<std::string> survivedMutants;

    std::string kernelFileName;
    std::stringstream notification;
    for (auto it = optionsParser.getSourcePathList().begin(); it!=optionsParser.getSourcePathList().end();it++){
        kernelFileName = it->c_str();
        kernels.push_back(kernelFileName);
    }

    int numKernels = kernels.size();
    if (numKernels == 0){
        notification << "Please provide at least one GPU kernel source code to process.";
        std::cout << ClmtUtils::colorString(notification.str(), output_color::KRED) << "\n";
        exit(error_code::KERNEL_FILE_NOT_PROVIDED);
    }

    expectedOutputFilePath = expectedOutput.c_str();
    int totalLines = ClmtUtils::getNumLines(expectedOutputFilePath);

    executableFilePath = executableFile.c_str();

    if (completeMatch && !matchLines.empty()){
        notification << "The matching mode should either be all (-all) or part match (specify lines in the format of \"3:5,7,-1\")";
        std::cout << ClmtUtils::colorString(notification.str(), output_color::KRED) << "\n";
        exit(error_code::LINES_TO_COMPARE_NOT_VALID);
    } 

    if (!matchLines.empty()){
        lines = ClmtUtils::retrieveNumbers(matchLines.c_str(), totalLines);
    } else {
        for (int i = 1; i <= totalLines; ++i){
            lines.push_back(i);
        }
    }

    UserConfig userConfig(userConfigFileName.c_str());
    for (auto itKernel = kernels.begin(); itKernel != kernels.end(); itKernel++){
        userConfig.generateFakeHeader(*itKernel);
    }
    clang::tooling::ClangTool tool(optionsParser.getCompilations(), optionsParser.getSourcePathList());
    std::map<std::string, std::list<std::string>>* mutantFileList;
    int numOperators = parseCode(&tool, numKernels, &mutantFileList);
    for (auto itKernel = kernels.begin(); itKernel != kernels.end(); itKernel++){
        UserConfig::removeFakeHeader(*itKernel);
    }
    notification << "Code invastigation summary: " << numOperators << " mutable operators have been found.";
    std::cout << ClmtUtils::colorString(notification.str(), output_color::KBLU) << "\n";
    notification.str("");

    notification << "Start executing with mutants";
    std::cout << ClmtUtils::colorString(notification.str(), output_color::KBLU) << "\n";

    std::string execCommand;
    int compareResult;
    for (auto itKernel = mutantFileList->begin(); itKernel != mutantFileList->end(); itKernel++){
        std::string currentKernel = itKernel->first;
        std::cout << "Kernel: " << currentKernel << "\n";
        execCommand = "mv " + currentKernel + " " + currentKernel + ".clmt_backup";
        system(execCommand.c_str());
        for (auto itMutant = itKernel->second.begin() ; itMutant != itKernel->second.end(); itMutant++){
            std::string currentMutant = *itMutant;
            std::cout << "  Running with mutant " << currentMutant << "\n";
            execCommand = "cp " + currentMutant + " " + currentKernel;
            system(execCommand.c_str());
            if (executableFilePath.substr(0,2).compare("./") == 0) execCommand = executableFilePath + " >> clmt_mutant_result.txt";
            else execCommand = "./" + executableFilePath + " >> clmt_mutant_result.txt";
            system(execCommand.c_str());

            //Compare
            if (ClmtUtils::fileEquals("clmt_mutant_result.txt", expectedOutputFilePath, lines)){
                survivedMutants.push_back(currentMutant);
            } else {
                execCommand = "rm " + currentMutant;
                system(execCommand.c_str());
            }
            execCommand = "rm clmt_mutant_result.txt";
            system(execCommand.c_str());
        }
        execCommand = "mv " + currentKernel + ".clmt_backup " + currentKernel;
        system(execCommand.c_str());
    }
    
    if (survivedMutants.size()){
        std::cout << "[Survived mutants]\n";
        for (auto itSurvivedMutants = survivedMutants.begin(); itSurvivedMutants != survivedMutants.end(); itSurvivedMutants++){
            std::cout << *itSurvivedMutants << "\n";
        }
    } else {
        std::cout << "No survived mutants\n";
    }

}
#include <string>
#include <map>
#include <list>
#include <iostream>
#include <sstream>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/Core/Replacement.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Basic/LLVM.h"

#include "OpenCLKernelInvastigator.h"
#include "Constants.h"
#include "Utils.h"

using namespace clang;
using namespace clang::tooling;

std::string codeTemplate;
std::map<std::string, std::string> templateMap;

std::map<std::string, unsigned int> operatorType;
std::map<std::string, std::list<std::string>> mutantOperator;

int currentOperator;
int numOperators;
int currentKernel;
int numKernels;
std::string currentFile;

class RecursiveASTVisitorForKerlInvastigator : public RecursiveASTVisitor<RecursiveASTVisitorForKerlInvastigator>{
public:
    explicit RecursiveASTVisitorForKerlInvastigator(Rewriter &r) : myRewriter(r) {}
    
    bool VisitStmt(Stmt *s){
        if (isa<BinaryOperator>(s)){
            BinaryOperator* binaryOperator = cast<BinaryOperator>(s);
            if (notRewritable(myRewriter.getSourceMgr(), binaryOperator->getOperatorLoc())) return true;
            std::string operatorStr = binaryOperator->getOpcodeStr().str() + "B";
            if (isMutable(operatorStr)){
                /*
                SourceLocation startLoc = myRewriter.getSourceMgr().getFileLoc(
                    binaryOperator->getLocStart());
                SourceLocation endLoc = myRewriter.getSourceMgr().getFileLoc(
                    binaryOperator->getLocEnd());
                SourceRange newRange;
                newRange.setBegin(startLoc);
                newRange.setEnd(endLoc);

                std::cout << "[debug]" << myRewriter.getRewrittenText(newRange) << std::endl;
                */

                std::stringstream operatorTemplate;
                operatorTemplate << "${operator_" << currentOperator << "_" << operatorStr << "}";
                myRewriter.ReplaceText(binaryOperator->getOperatorLoc(), operatorTemplate.str());
                currentOperator++;
                numOperators++;
            }
        } else if (isa<UnaryOperator>(s)){
            UnaryOperator* unaryOperator = cast<UnaryOperator>(s);
            if (notRewritable(myRewriter.getSourceMgr(), unaryOperator->getOperatorLoc())) return true;
            std::string operatorStr = unaryOperator->getOpcodeStr(unaryOperator->getOpcode()).str() + "U";
                        std::cout << "[operatorStr]" << operatorStr << std::endl;
            if (isMutable(operatorStr)){
                /*
                SourceLocation startLoc = myRewriter.getSourceMgr().getFileLoc(
                    unaryOperator->getLocStart());
                SourceLocation endLoc = myRewriter.getSourceMgr().getFileLoc(
                    unaryOperator->getLocEnd());
                SourceRange newRange;
                newRange.setBegin(startLoc);
                newRange.setEnd(endLoc);

                std::cout << "[debug]" << myRewriter.getRewrittenText(newRange) << std::endl;
                */

                std::stringstream operatorTemplate;
                operatorTemplate << "${operator_" << currentOperator << "_" << operatorStr << "}";
                myRewriter.ReplaceText(unaryOperator->getOperatorLoc(), operatorTemplate.str());
                currentOperator++;
                numOperators++;
            }
        }
        //TODO: instrument barriers 
        /*else if (isa<CallExpr>(s)){
            CallExpr *functionCall = cast<CallExpr>(s);
            SourceLocation startLoc = myRewriter.getSourceMgr().getFileLoc(
                    functionCall->getCallee()->getLocStart());
            SourceLocation endLoc = myRewriter.getSourceMgr().getFileLoc(
                    functionCall->getCallee()->getLocEnd());
            SourceRange newRange;
            newRange.setBegin(startLoc);
            newRange.setEnd(endLoc);
            std::string functionName = myRewriter.getRewrittenText(newRange);
            std::cout << "[debug]" << functionName << "\n";
        }*/
        return true;
    }
private:
    Rewriter &myRewriter;

    unsigned int getOperatorType(const std::string& operatorStr){
        if (operatorType.find(operatorStr) == operatorType.end()){
            return 0;
        } else {
            return operatorType[operatorStr];
        }
    }

    bool isMutable(const std::string& operatorStr){
        return (mutantOperator.find(operatorStr) == mutantOperator.end())? false: true;
    }

    bool notRewritable(const SourceManager& sm, const SourceLocation& sl){
        if (sm.isInExternCSystemHeader(sl)) return true;
        if (sm.isInSystemHeader(sl)) return true;
        if (sm.isMacroBodyExpansion(sl)) return true;
        if (sm.isMacroArgExpansion(sl)) return true;
        return false;
    }
};

class ASTConsumerForKernelInvastigator : public ASTConsumer{
public:
    ASTConsumerForKernelInvastigator(Rewriter &r): visitor(r) {}

    bool HandleTopLevelDecl(DeclGroupRef DR) override {
        for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
            // Traverse the declaration using our AST visitor.
            visitor.TraverseDecl(*b);
            //(*b)->dump();
        }
    return true;
    }

private:
    RecursiveASTVisitorForKerlInvastigator visitor;
};

class ASTFrontendActionForKernelInvastigator : public ASTFrontendAction {
public:
    ASTFrontendActionForKernelInvastigator() {}

    void EndSourceFileAction() override {
        const RewriteBuffer *buffer = myRewriter.getRewriteBufferFor(myRewriter.getSourceMgr().getMainFileID());
        if (buffer == NULL){
            llvm::outs() << "Rewriter buffer is null. Cannot write in file.\n";
            return;
        }
        std::string rewriteBuffer = std::string(buffer->begin(), buffer->end());
        std::string source;
        std::string line;
        std::istringstream bufferStream(rewriteBuffer);
        while(getline(bufferStream, line)){
            source.append(line);
            source.append("\n");
        }
        
        std::string filename = currentFile + "." + kernel_rewriter_constants::CODE_TEMPLATE_FILENAME_SUFFIX;
        std::ofstream outputFileStream(filename);
        outputFileStream << source;
        outputFileStream.close();
        UserConfig::removeFakeHeader(filename);
        ClmtUtils::generateMutant(filename, currentOperator - 1);
        currentOperator = 1;
        currentKernel++;
    }

    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &ci, 
        StringRef file) override {
            std::stringstream notification;
            notification << "[" << currentKernel << "/" << numKernels << "] Instrumenting " << file.str();
            currentFile = file.str();
            std::cout << ClmtUtils::colorString(notification.str(), output_color::KBLU) << "\n";
            myRewriter.setSourceMgr(ci.getSourceManager(), ci.getLangOpts());
            return llvm::make_unique<ASTConsumerForKernelInvastigator>(myRewriter);
        }

private:
    Rewriter myRewriter;
};

int parseCode(clang::tooling::ClangTool* tool, const int& numKernelsIn){
    codeTemplate = "";
    templateMap.clear();
    ClmtUtils::initialiseOperatorTypeMap(operatorType);
    ClmtUtils::initialiseMutantOperatorMap(mutantOperator);
    currentOperator = 1;
    numKernels = numKernelsIn;
    currentKernel = 1;
    numOperators=0;

    tool->run(newFrontendActionFactory<ASTFrontendActionForKernelInvastigator>().get());
    return numOperators;
}
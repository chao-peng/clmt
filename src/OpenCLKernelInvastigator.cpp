#include <string>
#include <map>
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

class RecursiveASTVisitorForKerlInvastigator : public RecursiveASTVisitor<RecursiveASTVisitorForKerlInvastigator>{
public:
    explicit RecursiveASTVisitorForKerlInvastigator(Rewriter &r) : myRewriter(r) {}
    
    bool VisitStmt(Stmt *s){
        // If
        if (isa<BinaryOperator>(s)){
            BinaryOperator* binaryOperator = cast<BinaryOperator>(s);
            std::string operatorStr = binaryOperator->getOpcodeStr().str();
            if (ClmtUtils::isMutable(operatorStr)){
                myRewriter.ReplaceText(binaryOperator->getOperatorLoc(),"${operator}");
            }
            std::string debug = myRewriter.getRewrittenText(binaryOperator->getSourceRange());
            std::cout << operatorStr << " " << debug << std::endl;
        } else if (isa<UnaryOperator>(s)){
            UnaryOperator* unaryOperator = cast<UnaryOperator>(s);
            std::string debug = myRewriter.getRewrittenText(unaryOperator->getSourceRange());
            std::cout << debug << std::endl;
        }
        return true;
    }
private:
    Rewriter &myRewriter;
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

        std::cout << source << std::endl;

    }

    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &ci, 
        StringRef file) override {
            myRewriter.setSourceMgr(ci.getSourceManager(), ci.getLangOpts());
            return llvm::make_unique<ASTConsumerForKernelInvastigator>(myRewriter);
        }

private:
    Rewriter myRewriter;
};

int parseCode(clang::tooling::ClangTool* tool){
    codeTemplate = "";
    templateMap.clear();

    tool->run(newFrontendActionFactory<ASTFrontendActionForKernelInvastigator>().get());
    return error_code::STATUS_OK;
}
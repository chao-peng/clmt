#ifndef CLMT_REWRITABLE_CODE_HEADER_
#define CLMT_REWRITABLE_CODE_HEADER_

#include <string>

#include "Constants.h"

class RewritableCode{
    public:
    RewritableCode();
    RewritableCode(std::string code);
    std::string generateCode();
    void feedCode(std::string code);
    
    int getNumBarrier();
    int getNumConditions();
    int getNumExpressions();
    int enableBarrier(int barrierID);
    int disableBarrier(int barrierID);
    int changeBarrierArgument(int barrierID, int argumentType);
    int gettBarrierArgument(int barrierID);
};

#endif

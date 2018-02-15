#ifndef CLMT_CONSTANTS_HEADER_
#define CLMT_CONSTANTS_HEADER_

namespace error_code{
    const int KERNEL_FILE_NOT_PROVIDED = -1;
    const int LINES_TO_COMPARE_NOT_VALID = -2;
}

namespace barrier_type{
    const unsigned int LOCAL = 0x01;
    const unsigned int GLOBAL = 0x02;
    const unsigned int LOCAL_AND_GLOBAL = 0x03;
}

#endif
#ifndef CLMT_CONSTANTS_HEADER_
#define CLMT_CONSTANTS_HEADER_

namespace kernel_rewriter_constants{
    const char* const FAKE_HEADER_MACRO = "OPENCLBC_FAKE_HEADER_FOR_LIBTOOLING_";
}

namespace error_code{
    const int STATUS_OK = 0;
    const int TWO_MANY_HOST_FILE_SUPPLIED = 1;
    const int NO_HOST_FILE_SUPPLIED = 2;
    const int REMOVE_KERNEL_FAKE_HEADER_FAILED_KERNEL_DOES_NOT_EXIST = 3;
    const int KERNEL_FILE_ALREADY_HAS_FAKE_HEADER = 4;
    const int NO_NEED_TO_TEST_COVERAGE = 5;
    const int KERNEL_FILE_NOT_PROVIDED = 6;
    const int LINES_TO_COMPARE_NOT_VALID = 7;
}

namespace barrier_type{
    const unsigned int LOCAL = 0x01;
    const unsigned int GLOBAL = 0x02;
    const unsigned int LOCAL_AND_GLOBAL = 0x03;
}

#endif
set(LLVM_LINK_COMPONENTS
    Support
)
        
add_clang_executable(clmt
    src/Main.cpp)
        
target_link_libraries(clmt
    clangAST
    clangASTMatchers
    clangBasic
    clangFrontend
    clangTooling)

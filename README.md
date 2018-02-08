# clmt: Mutation testing OpenCL kernels

## Introduction

Based on Clang Libtooling technique, clmt is developped to conduct mutation testing on OpenCL kernels.

## Folder Structure

* **src** Source code

## Build

1. To build and use this tool, please first follow the instructions on how to build Clang/LLVM on your machine

    [http://clang.llvm.org/docs/LibASTMatchersTutorial.html](http://clang.llvm.org/docs/LibASTMatchersTutorial.html)

2. Clone clmt to `~/clang-llvm/llvm/tools/clang/tools/extra`

3. Add the following line to `~/clang-llvm/llvm/tools/clang/tools/extra/CMakeLists.txt`

```
    add_subdirectory(clmt)
```

4. Build clmt

```bash
    cd ~/clang-llvm/build
    ninja
```

## Usage

The tool is still under development.


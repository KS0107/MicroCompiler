# Micro Compiler

This project is a simple compiler for the micro language written with primarily C++. furthering the work of Matthew Oros.
This version of the Micro compiler is designed for use with devices using an ARM64 instruction set, which means it may not run on x86 or other architectures without modification.

## Features

Tokenizes input source code
Parses tokens into an abstract syntax tree (AST)
Generates assembly code from the AST
Assembles and links the generated assembly into an executable

## Requirements
CMake (for building the project)
C++ compiler (e.g., g++)
ARM64 architecture (for running the generated executable)
GNU assembler (as)
GNU linker (ld)
macOS version 14.0 or later (for specific linking options)

## Usage

1. cd to root directory of the project and create a build directory: ```mkdir build```
2. Use CMake to generate all build files: ```cmake -S . -B build```
3. Build using CMake: ```cmake --build build```
4. If you have a source file named test.micro, you can run it using ```./build/microcompiler test.micro```
5. The generated executable file will automatically be executed once compiled

## Testing

1. Running cmake withing the build dir is also going to build and run the test suite 

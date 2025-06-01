# Reaver C Compiler

[![Linux](https://github.com/saturn691/ReaverCompiler/actions/workflows/ci-linux.yml/badge.svg)](https://github.com/saturn691/ReaverCompiler/actions/workflows/ci-linux.yml) [![Mac OS X](https://github.com/saturn691/ReaverCompiler/actions/workflows/ci-macos.yml/badge.svg)](https://github.com/saturn691/ReaverCompiler/actions/workflows/ci-macos.yml)

Reaver C Compiler (`rcc`) is a C99 compiler. It features a lexer, parser,
preprocessor, AST, and compiles to LLVM IR. It supports both x86 and AArch64.

## Why?

For fun! We started [`v1.0`](https://github.com/saturn691/ReaverCompiler/tree/v1.0)
as part of our 2nd year IAC coursework at Imperial College London. Then, to
learn about LLVM, I decided to rewrite the entire compiler. For more 
information on the evolution from
[`v1.0`](https://github.com/saturn691/ReaverCompiler/tree/v1.0), please read
[`info.md`](docs/info.md).

The code here can be used as a guide to get started on writing an LLVM based
toy compiler.

## Getting started

> [!NOTE]
> This has been tested only on Linux (x86-64) and Mac (AArch64). There is no
> planned support for Windows or other architectures.
 
### Dependencies

Reaver C Compiler relies on the following dependencies:

- LLVM
- CLI11
- Boost
- GTest

It also relies on the following tools:

- CMake 3.20+
- Clang (if linking is required)
- Git
- Ninja
- Python
- Flex
- Bison 3.0+

### Building from source

1. [Optional] Clone the external repos into a folder called `third_party`. 
   This will take some time. It is possible to use the latest branch, however,
   the compiler has only been tested on these versions.

```bash
mkdir third_party
git -C third_party clone --branch llvmorg-20.1.5 git@github.com:llvm/llvm-project.git
git -C third_party clone --branch boost-1.88.0 --recursive git@github.com:boostorg/boost.git
git -C third_party clone --branch v2.5.0 git@github.com:CLIUtils/CLI11.git
git -C third_party clone --branch v1.17.0 git@github.com:google/googletest.git
```

2. Run CMake.

```bash
mkdir build
cd build
cmake .. \
    -G Ninja \
    -DFETCHCONTENT_SOURCE_DIR_LLVM_PROJECT=../third_party/llvm-project \
    -DFETCHCONTENT_SOURCE_DIR_BOOST=../third_party/boost \
    -DFETCHCONTENT_SOURCE_DIR_CLI11=../third_party/CLI11 \
    -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=../third_party/googletest
ninja
```

### Usage

To run the compiler, run the following command, replacing the flags,

- `-o` for the output file path

For example, this will compile the example program:

```bash
build/rcc ./tests/_example/example.c -o ./example.o
```

Then, invoke your linker to link the object file.

```bash
clang ./tests/_example/example_driver.c example.o
```

### Running Unit Tests

To run all unit tests, run the following commands:

```bash
cd build
ctest
```

### Running Integration Tests

To run the provided integration tests, run the following command:

```bash
./test.py
```

To run the additional integration tests, run the following commands:

```bash
# Only run this once
git submodule update --init --recursive
# More options available. Does not pass all tests yet
./writing-a-c-compiler-tests/test_compiler --chapter 18 --skip-invalid build/rcc
```

## Credits

- [William Huynh](https://www.linkedin.com/in/wh691/)
- [Kevin Lau](https://www.linkedin.com/in/kevinlau01/)

## References

C99 standard: https://port70.net/%7Ensz/c/c99/n1256.html

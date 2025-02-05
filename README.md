# Reaver C Compiler

Reaver C Compiler (`rcc`) is a C99 compiler.

To browse the compiler at the time of submission, which scored 90% (181/201),
navigate to branch [`v1.0`](https://github.com/saturn691/ReaverCompiler/tree/v1.0).

This compiler features a lexer, parser, preprocessor, AST, and compiles to LLVM IR.
It passes all 201/201 tests from the original coursework and 480+ additional tests.

For more information, please read [`info.md`](docs/info.md).

## Getting started

### Dependencies

To install dependencies, run the following commands:

```bash
sudo apt update && xargs -a dependencies.txt sudo apt install -y

# On some systems, there are issues with libc++abi. To fix, run:
sudo apt purge libc++abi-18-dev
```

### Usage

To make the compiler, run the following commands:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

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

## Credits

- [William Huynh](https://www.linkedin.com/in/wh691/)
- [Kevin Lau](https://www.linkedin.com/in/kevinlau01/)

## References

C99 standard: https://port70.net/%7Ensz/c/c99/n1256.html

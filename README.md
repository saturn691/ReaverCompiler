# Reaver C Compiler

Reaver C Compiler (`rcc`) is a C99 compiler.

To browse the compiler at the time of submission, which scored 90% (181/201),
navigate to branch [`v1.0`](https://github.com/saturn691/ReaverCompiler/tree/v1.0).

## Getting started

### Dependencies

To install dependencies, run the following command:

```bash
bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
sudo apt install flex bison
```

### Usage

To make the compiler, run the following command:

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

## Credits

- [William Huynh](https://www.linkedin.com/in/wh691/)
- [Kevin Lau](https://www.linkedin.com/in/kevinlau01/)

## References

C99 standard: https://port70.net/%7Ensz/c/c99/n1256.html

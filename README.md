# Reaver C Compiler

Reaver C Compiler (`rcc`) is a C90 compiler.

To browse the compiler at the time of submission, which scored 90\% (181/201),
navigate to tag [`v1.0`](https://github.com/saturn691/ReaverCompiler/tree/v1.0).
The development was largely driven by the [specification](docs/c_compiler.md).

Changes from `v1.0`:
- Targets all architectures, not only RISC-V by using a LLVM backend.
- Fixes to globals, enums, structs, arrays and pointers.

## Getting started

### Dependencies

To install the dependencies, run the following command:

```
bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
```

### Usage

To make the compiler, run the following command:

```bash
mkdir build
cmake ..
cmake --build .
```

To run the compiler, run the following command, replacing the flags,

- `-S` for the input file path
- `-o` for the output file path

For example, this will compile the example program:

```bash
bin/rcc -S "./compiler_tests/_example/example.c" -o "./example.s"
```

## Credits

- [William Huynh](https://www.linkedin.com/in/wh691/)
- [Kevin Lau](https://www.linkedin.com/in/kevinlau01/)

## References

C99 standard: https://port70.net/%7Ensz/c/c99/n1256.html

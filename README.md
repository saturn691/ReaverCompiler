# Reaver C Compiler

Reaver C Compiler (`rcc`) is a C99 compiler.

To browse the compiler at the time of submission, which scored 90% (181/201),
navigate to branch [`v1.0`](https://github.com/saturn691/ReaverCompiler/tree/v1.0).

## Getting started

### Usage

To make the compiler, run the following command:

```bash
mkdir build
cd build
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

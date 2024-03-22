# Reaver C Compiler

Reaver C Compiler (`rcc`) is a C90 to RISC-V compiler. The development was
largely driven by the [specification](docs/c_compiler.md).

## Getting started

### Dependencies

The dependencies are listed in the [Dockerfile](Dockerfile).

### Usage

To make the compiler, run the following command:

```bash
make
```

To run the compiler, run the following command, replacing the flags,

- `-S` for the input file path
- `-o` for the output file path

For example, this will compile the example program:

```bash
bin/c_compiler -S "./compiler_tests/_example/example.c" -o "./example.s"
```

## Credits

- [William Huynh](https://www.linkedin.com/in/wh691/)
- [Kevin Lau](https://www.linkedin.com/in/kevinlau01/)

## References

ANSI C90 standard: https://www.dii.uchile.cl/~daespino/files/Iso_C_1999_definition.pdf

RISC-V Calling Convention: https://riscv.org/wp-content/uploads/2015/01/riscv-calling.pdf

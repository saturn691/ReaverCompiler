# Kevin's Logbook (Time-stamped)

## Week of 17th Dec 2023

### 22/12
- New branch created, attempts of resolving Makefile and lexer/parser issues

### 23/12
- Branch deleted, Makefile issues temporarily resolved by William, `example.c` runs

## Week of 24th Dec 2023

### 25/12
- Created a pull request to merge branch `kevin`, but waiting for William's AST branch to be working to merge
- Team goal set for Friday:
  - William: AST working and nodes defined properly
  - Kevin: Obtain a scalable IR generator on another project
- Steps for IR generator:
  - What type of IR code should we use?
  - TAC (three address code), because it helps bridge the gap between high-level language to lower-level assembly code, making translation to machine code easier
  - Create a translation table for TAC to RISC-V assembly
  - Find out how to create TAC
- Design choice:
  - Make multiple header files for each type of node, using object-oriented principles, and letting each keyword inherit the type of node

### 26/12
- Added printing functionalities for required `operators` and `relops` in the `compiler_tests/integer` file
- Went over the functionalities of most functions in various parts of the code, including getting a better idea of traversing the tree
- Current main issues:
  - Printing does not print all the keywords, such as the semicolon
  - `gen_asm` does not work for anything except `_example/example.c`

### 27/12
- Traced through the entire AST and successfully implemented `local_var/constant_initialiser` and `local_var/expression_initialiser`
- Important notes:
  - Use breakpoint + F5 to debug, such as use it to check related `gen_asm` functions from other `.hpp` files
  - Make sure to check the `type` of node before debugging - it helps understand the expected behaviour of the node

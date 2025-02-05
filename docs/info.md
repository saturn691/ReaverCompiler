# Reaver's Design

## Evolution from v1.0

### Prelude

The compiler journey begins with ReaverCompiler 
[`v1.0`](https://github.com/saturn691/ReaverCompiler/tree/v1.0), developed 
from December 2023 to January 2024. As part of the coursework for Instruction 
Architectures and Compilers at Imperial College London, Kevin and I wrote the 
original compiler, which compiled C90 to RISC-V.

### Design

The original compiler was very simple. After the C code was read from a file,
lexed (with Flex), parsed (with Bison), simple DFS inspired algorithms were used
to lower into assembly. Below is a simplified approach to lower a (+) node.

```cpp
// destReg was a0, at the top level
std::string lhsReg = context.allocate_temp_reg();
std::string rhsReg = context.allocate_temp_reg();

lhs->lower(lhsReg, context);
rhs->lower(rhsReg, context)
std::cout << "add " << destReg << ", " << lhsReg << ", " << rhsReg; 

context.free_reg(lhsReg);
context.free_reg(rhsReg);
```

And that's that! Nothing fancy; we just put all contextual information into
`Context`, only use one pass, print out the assembly to a file directly.
C is basically just assembly, right?

### Problems

Unfortunately, as you may have guessed, doing everything in one place violates
everything that OOP (and SOLID) stood for! As C is a context-sensitive language,
it makes it even harder to run everything through one pass. A classic one is
the `typedef` lexer hack. See the original context file:
[ast_context.hpp](https://github.com/saturn691/ReaverCompiler/blob/v1.0/include/ast/ast_context.hpp).
The worst offender there must have been the `Mode` which stores information on the
previously visited node 😭. I have kept Simon's (the TA's) wonderful feedback 
blasting every design choice in [`feedback.md`](feedback.md).

In the end, it just ended up as spaghetti code, which ended up being too hard to
build on. A radical rethink had to be done.

## Improvements in v2.0

Whilst the general gist of the compiler is the same, I spent a couple of days
architecting the structure. I looked at cppreference, clang, rustc and many
other resources while planning. Below I have listed the major improvements:

### File Structure

One of the things I hated the most was the amount of files; for every single node
in the AST there was a file. Whilst this may cut down compile times, it made
navigating so much harder. Now, the 50+ files have been reduced to 8 (in 
[`include/AST`](../include/AST)).

### Visitor design

Instead of adding more virtual functions, I added a single `visit()` function
to every AST node, allowing an easy pipeline of information to code generation.
This also allows the [`Printer`](../include/AST/Printer.hpp) to verify my parser,
and allows separation of the AST/CodeGen logic.

### Better C++ practices

Modern C++ is weird, but also quite neat. I make use of `std::variant` and
`std::visit` a lot. I've used CRTP (curiously recurring template pattern) and
tried to write code that mimics LLVM. Most importantly, I've ditched
`snake_case` and gone back to `camelCase` like the good ol' days.

### More layers

An addition of a preprocessor and a type checker/semantic analyzer! Why not?

### Better language support

More language support! Abstract declarators, struct ABI handling, arrays done
properly, storage class specifiers, and loads more!

## Next steps

Working on ReaverCompiler `v2.0` has been such a joy, given its structure.
Whilst the C99 compiler is not exactly feature complete, it does push its way
through some insane test cases in `writing-a-c-compiler-tests` and passes all
the test cases given in the IAC module.

For me, it's time to take my knowledge from this journey and apply it somewhere
else, whether it's in compiler optimizations, writing a compiler backend, or
doing something completely different. You are very welcome to fork this
compiler to develop it, or just use it as inspiration for your own.

## Acknowledgements

I would like to thank [John Wickerson](https://github.com/johnwickerson)
for designing and teaching the module which kickstarted my journey into
compilers and my lab partner, [Kevin](https://github.com/booth-algo) for 
writing half of [`v1.0`](https://github.com/saturn691/ReaverCompiler/tree/v1.0).
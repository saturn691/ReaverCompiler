// Define necessary includes for LLVM functionality.
%{
#include <cstdio>
#include <iostream>
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

// Define LLVM context, module, and IRBuilder.
llvm::LLVMContext TheContext;
llvm::IRBuilder<> Builder(TheContext);
std::unique_ptr<llvm::Module> TheModule;

extern int yylex();
void yyerror(const char *s) { std::cout << "Error: " << s << std::endl; }
%}

// Declare a union for semantic values. This helps in handling different data types like LLVM values or integers.
%union {
    llvm::Value* value;
    int intVal;
}

// Declare tokens and their associated types.
%token <intVal> DIGIT
%token IDENTIFIER
%type <value> function statement

// Start rule for the grammar is "function".
%start function

%%

// Rule for parsing a C function definition.
function: 
    "int" IDENTIFIER '(' ')' '{' statement '}'  {
        std::cout << "Function rule triggered. Statement value: " << $<value>7 << std::endl;
        $$ = $<value>7;
    }
;


// Rule for a return statement.
statement: 
    "return" DIGIT ';'   // Matches the return statement structure.
    { 
        // This block is executed after a return statement is recognized.
        // A new LLVM constant integer is created with the value provided and is set as the statement's value.
        $$ = llvm::ConstantInt::get(TheContext, llvm::APInt(32, $2)); 
    }
;

%%

// Required function to start parsing when the parser is invoked.
int yywrap() { return 1; }

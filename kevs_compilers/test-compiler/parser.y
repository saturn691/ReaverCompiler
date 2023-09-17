%{
#include <stdio.h>
#include <stdlib.h>
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <vector>

extern int yylex();
void yyerror(const char* s) {
    fprintf(stderr, "Parse error: %s\n", s);
    exit(1);
}

llvm::LLVMContext TheContext;
llvm::IRBuilder<> Builder(TheContext);
std::unique_ptr<llvm::Module> TheModule;
llvm::Function *TheFunction;

int yylval;
%}

%token DIGIT IDENTIFIER

%%

program:
    function_definition
    {
        TheModule->print(llvm::errs(), nullptr);
    }
    ;

function_definition:
    "int" IDENTIFIER '(' ')' '{' statement '}' 
    {
        TheFunction = llvm::Function::Create(
            llvm::FunctionType::get(llvm::Type::getInt32Ty(TheContext), false),
            llvm::Function::ExternalLinkage,
            "f", *TheModule
        );
        llvm::BasicBlock *BB = llvm::BasicBlock::Create(TheContext, "entry", TheFunction);
        Builder.SetInsertPoint(BB);
        Builder.CreateRet($7);
    }
    ;

statement:
    "return" DIGIT ';'   { $$ = llvm::ConstantInt::get(TheContext, llvm::APInt(32, yylval)); }
    ;

%%


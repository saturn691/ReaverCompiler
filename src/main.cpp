#include "AST/AST.hpp"
#include "CLI/CLI.hpp"
#include "CodeGen/CodeGenModule.hpp"
#include "CodeGen/TypeChecker.hpp"
#include <iostream>

void compile(
    const std::string &sourcePath,
    const std::string &out,
    bool emitLLVM)
{
    const AST::TranslationUnit *tu = AST::parseAST(sourcePath);

    // Print the AST
    AST::Printer printer(std::cout);
    tu->accept(printer);

    // Type check the AST
    CodeGen::TypeChecker typeChecker(std::cerr);
    tu->accept(typeChecker);

    // Code generation
    CodeGen::CodeGenModule CGM(out, typeChecker.getTypeMap());
    tu->accept(CGM);

    if (emitLLVM)
    {
        // Print the LLVM IR
        CGM.emitLLVM();
    }
    else
    {
        CGM.emitObject();
    }
}

int main(int argc, char **argv)
{
    CLI::App app;
    std::string sourcePath;
    std::string outputPath = "output.o";
    bool emitLLVM = false;

    // Options for the CLI

    // Add positional argument
    app.add_option("source", sourcePath, "Source file path")
        ->required()
        ->check(CLI::ExistingFile);
    app.add_option("-o", outputPath, "Output file path")->capture_default_str();
    app.add_flag("-S", emitLLVM, "Emit LLVM IR instead of object code");

    CLI11_PARSE(app, argc, argv);

    // Compile the input
    std::cout << "Compiling: " << sourcePath << std::endl;
    compile(sourcePath, outputPath, emitLLVM);
    std::cout << "Compiled to: " << outputPath << std::endl;

    return 0;
}
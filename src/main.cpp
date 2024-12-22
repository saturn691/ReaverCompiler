#include "AST/AST.hpp"
#include "CLI/CLI.hpp"
#include "CodeGen/CodeGenModule.hpp"
#include "CodeGen/TypeChecker.hpp"
#include <iostream>

void compile(std::string &sourcePath, std::ostream &out)
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

    // Print the LLVM IR
    CGM.print();
}

int main(int argc, char **argv)
{
    CLI::App app;
    std::string sourcePath, outputPath;

    // Options for the CLI
    app.add_option("-S", sourcePath, "Source file path")
        ->required()
        ->check(CLI::ExistingFile);
    app.add_option("-o", outputPath, "Output file path")->required();

    CLI11_PARSE(app, argc, argv);

    // Open the output file in truncation mode (to overwrite the contents)
    std::ofstream output;
    output.open(outputPath, std::ios::trunc);

    // Compile the input
    std::cout << "Compiling: " << sourcePath << std::endl;
    compile(sourcePath, output);
    std::cout << "Compiled to: " << outputPath << std::endl;

    output.close();
    return 0;
}
#include "AST/AST.hpp"
#include "CLI/CLI.hpp"
#include "CodeGen/CodeGenModule.hpp"
#include "CodeGen/TypeChecker.hpp"

#include <boost/filesystem.hpp>
#include <cstdio>
#include <iostream>

#define TCPP_IMPLEMENTATION
#include "Preprocessor.hpp"

std::string preprocess(const std::string &sourcePath)
{
    // Open sourcePath, spill contents as std::string
    std::ifstream ifs(sourcePath);
    std::string sourceContents(
        (std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>()));

    auto errorCallback = [](const tcpp::TErrorInfo &msg) {};
    tcpp::Preprocessor::TOnIncludeCallback includeCallback =
        [&includeCallback, &errorCallback, &sourcePath](
            const std::string &path,
            bool isSystem) -> tcpp::TInputStreamUniquePtr
    {
        // Search sourcePath's directory for the include file
        boost::filesystem::path sourceDir =
            boost::filesystem::path(sourcePath).parent_path();
        boost::filesystem::path includePath = sourceDir / path;

        std::string processedCPath = preprocess(includePath.string());
        std::ifstream ifs(processedCPath);
        std::string processedC(
            (std::istreambuf_iterator<char>(ifs)),
            (std::istreambuf_iterator<char>()));

        return std::make_unique<tcpp::StringInputStream>(processedC);
    };
    tcpp::Lexer lexer(
        std::make_unique<tcpp::StringInputStream>(sourceContents));
    tcpp::Preprocessor preprocessor(lexer, {errorCallback, includeCallback});

    std::string processedC = preprocessor.Process();

    // Create a file with the preprocessed contents
    auto tempFile = boost::filesystem::temp_directory_path() /
                    boost::filesystem::unique_path();
    {
        std::ofstream ofs(tempFile.string());
        ofs << processedC;
    }

    return tempFile.string();
}

void compile(
    const std::string &sourcePath,
    const std::string &out,
    bool emitLLVM)
{
    // Preprocess the input
    std::string preprocessedPath = preprocess(sourcePath);

    // Parse the AST
    const AST::TranslationUnit *tu = AST::parseAST(preprocessedPath);

    // Print the AST
    AST::Printer printer(std::cout);
    tu->accept(printer);

    // Type check the AST
    CodeGen::TypeChecker typeChecker(std::cerr);
    tu->accept(typeChecker);

    // Code generation
    CodeGen::CodeGenModule CGM(sourcePath, out, typeChecker.getTypeMap());
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
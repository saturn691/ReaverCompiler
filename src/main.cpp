#include "AST/AST.hpp"
#include "CLI/CLI.hpp"
#include "CodeGen/CodeGenModule.hpp"
#include "CodeGen/TypeChecker.hpp"

#include <boost/filesystem.hpp>
#include <cstdio>
#include <iostream>

#define TCPP_IMPLEMENTATION
#include "Preprocessor.hpp"

void preprocess(const std::string &sourcePath, const std::string &outputPath)
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

        auto tempFile = boost::filesystem::temp_directory_path() /
                        boost::filesystem::unique_path();
        std::string processedCPath = tempFile.string();
        preprocess(includePath.string(), processedCPath);
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

    {
        std::ofstream ofs(outputPath);
        ofs << processedC;
    }
}

void link(const std::string &sourcePath, const std::string &outputPath)
{
    FILE *pipe = popen("which clang", "r");
    if (!pipe)
    {
        std::cerr << "Error: failed to run which clang\n";
        return;
    }

    char buffer[256];
    std::string clangPath;
    if (fgets(buffer, sizeof(buffer), pipe))
    {
        clangPath = buffer;
        clangPath.erase(
            std::remove(clangPath.begin(), clangPath.end(), '\n'),
            clangPath.end());
    }
    pclose(pipe);

    if (clangPath.empty())
    {
        std::cerr << "Error: clang not found in PATH\n";
        return;
    }

    std::cout << "Invoking: " << clangPath << std::endl;

    // Calling std::system is not best practice, however, it works here
    std::string cmd = clangPath + " " + sourcePath + " -o " + outputPath;
    if (std::system(cmd.c_str()) != 0)
    {
        std::cerr << "Error: clang invocation failed\n";
    }
}

void compile(
    const std::string &sourcePath,
    const std::string &outputPath,
    const std::string &targetTriple,
    bool emitLLVM,
    bool useLinker,
    bool print)
{
    bool needLinker = useLinker && !emitLLVM;
    std::string outputPathCGM = outputPath;

    // Temporary file needed. *.c -> *.o -> a.out
    if (needLinker)
    {
        auto tempFile = boost::filesystem::temp_directory_path() /
                        boost::filesystem::unique_path();
        outputPathCGM = tempFile.string();
    }

    auto tempFile = boost::filesystem::temp_directory_path() /
                    boost::filesystem::unique_path();
    std::string preprocessedPath = tempFile.string();

    // Preprocess the input
    preprocess(sourcePath, preprocessedPath);

    // Parse the AST
    const AST::TranslationUnit *tu = AST::parseAST(preprocessedPath);

    if (print)
    {
        AST::Printer printer(std::cout);
        tu->accept(printer);
    }

    // Type check the AST
    CodeGen::TypeChecker typeChecker(std::cerr);
    tu->accept(typeChecker);

    // Code generation
    CodeGen::CodeGenModule CGM(
        sourcePath, outputPathCGM, typeChecker.getTypeMap(), targetTriple);
    tu->accept(CGM);

    if (emitLLVM)
    {
        CGM.emitLLVM();
    }
    else
    {
        CGM.emitObject();
    }

    // Link (if possible)
    if (needLinker)
    {
        link(outputPathCGM, outputPath);
    }
}

int main(int argc, char **argv)
{
    CLI::App app;
    std::string sourcePath;
    std::string outputPath;
    std::string targetTriple;
    bool emitLLVM = false;
    bool noLink = false;
    bool print = false;

    // Options for the CLI

    // Add positional argument
    app.add_option("source", sourcePath, "Source file path")
        ->required()
        ->check(CLI::ExistingFile);
    app.add_option("-o", outputPath, "Output file path");
    app.add_flag(
        "-c", noLink, "Only run preprocess, compile and assemble steps");
    app.add_flag("-S", emitLLVM, "Emit LLVM IR instead of object code");
    app.add_flag("-v", print, "Show parser output");
    app.add_flag(
        "--target", targetTriple, "Generate code for the given target");

    CLI11_PARSE(app, argc, argv);

    // Follows conventions set by clang
    if (outputPath.empty())
    {
        std::filesystem::path p{sourcePath};
        std::string stem = p.stem().string();

        // Descending order: assembly -> executable
        if (emitLLVM)
        {
            outputPath = stem + ".ll";
        }
        else if (noLink)
        {
            outputPath = stem + ".o";
        }
        else
        {
            outputPath = "a.out";
        }
    }

    std::cout << "Compiling: " << sourcePath << std::endl;
    compile(sourcePath, outputPath, targetTriple, emitLLVM, !noLink, print);
    std::cout << "Compiled to: " << outputPath << std::endl;

    return 0;
}
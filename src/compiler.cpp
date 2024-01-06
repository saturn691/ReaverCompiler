#include <fstream>
#include <iostream>
#include <unistd.h>

#include "cli.h"
#include "ast.hpp"


void compile(std::string sourcePath, std::ostream &out)
{
    // Generate the AST
    const Node *ast = parseAST(sourcePath);

    // Print out the AST to stdout
    ast->print(std::cout, 0);
    std::cout << std::endl;

    // Compiile the output into the file
    Context context;

    // This doesn't matter- will either be a0 or fa0
    std::string output_register = "";

    ast->gen_asm(out, output_register, context);
}


extern FILE *yyin;


int main(int argc, char **argv)
{
    // Parse CLI arguments, to fetch the values of the source and output files.
    std::string sourcePath = "";
    std::string outputPath = "";
    if (parse_command_line_args(argc, argv, sourcePath, outputPath))
    {
        return 1;
    }

    // This configures Flex to look at sourcePath instead of reading from stdin.
    yyin = fopen(sourcePath.c_str(), "r");
    if (yyin == NULL)
    {
        perror("Could not open source file");
        return 1;
    }

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

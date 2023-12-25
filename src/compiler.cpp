#include <fstream>
#include <iostream>
#include <unistd.h>

#include "cli.h"

void compile(std::ostream &w)
{
    w << ".text" << std::endl;
    w << ".globl f" << std::endl;
    w << std::endl;

    w << "f:" << std::endl;
    w << "addi  t0, zero, 0" << std::endl;
    w << "addi  t0, t0,   5" << std::endl;
    w << "add   a0, zero, t0" << std::endl;
    w << "ret" << std::endl;
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
    compile(output);
    std::cout << "Compiled to: " << outputPath << std::endl;

    output.close();
    return 0;
}

// Dot file generation
void gen_dot_file(struct node *tree)
{
    FILE *fp = fopen("ast.dot", "w");

    if (fp = NULL){
        perror("Could not open file");
        return;
    }

    // Start and generate DOT graph
    fprintf(fp, "digraph AST {\n");
    print_dot_file(tree, fp);

    // Close dot file
    fprintf(fp, "}\n");
    fclose(fp);
}

// Print AST in dot format by traversing
void print_dot(struct node *tree, FILE *fp) {
    if (tree == NULL)
        return;

    // Print node token
    fprintf(fp, "\"%p\" [label=\"%s\"];\n", tree->token, tree->token);

    // Print left child (recursive print)
    if (tree->left != NULL) {
        fprintf(fp, "\"%p\" [label=\"%s\"];\n", tree->token, tree->left->token);
        print_dot(tree->left, fp);
    }

    // Print right child (recursive print)
    if (tree->right != NULL) {
        fprintf(fp, "\"%p\" [label=\"%s\"];\n", tree->token, tree->right->token);
        print_dot(tree->right, fp);
    }
}


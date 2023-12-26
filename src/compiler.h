#ifndef COMPILER_H
#define COMPILER_H

#include "parser.tab.h" // not sure if needed

// Graphviz dot file generation
void gen_dot_file(struct node *tree);
void print_dot(struct node *tree, FILE *fp);

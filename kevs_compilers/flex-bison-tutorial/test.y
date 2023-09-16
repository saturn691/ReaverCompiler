%{

#include <stdio.h>

int yylex();
int yyerror(char *s);

%}

%token STRING NUM OTHER SEMICOLON PIC

%type <name> STRING
%type <number> NUM
%type <name> PIC

%union{
    char name[20];
    int numdata;
}

%%

prog:
    stmts
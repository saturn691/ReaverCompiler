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
    int number;
}
/* union: bridge between lex and yacc code */

%%

prog:
    stmts
;

stmts:
    | stmt SEMICOLON stmts

stmt:
    STRING {
        printf("You entered a string - %s", $1);
        // the dollar sign 1 is referring to the type if there is more than one type above
        // %s print as char
    }
    | NUM {
        printf("The number you entered is - %d", $1);
        // %d print as integer
    }
    | OTHER
;

%%

int yyerror(char *s)
{
    printf("Syntax Error on line %s\n", s);
    return 0;
}

int main()
{
    yyparse();
    return 0;
}

/* Definition section */
%option noyywrap
%{
    #include <stdio.h>
    // #include "parser.tab.hpp"
%}

DIGIT               [0-9]
LETTER              [a-zA-Z]
IDENTIFIER          {LETTER}({LETTER}|{DIGIT})*
NUMBER              {DIGIT}+
HEXADECIMAL         [a-fA-F0-9]
EXPONENT            [Ee][+-]?{DIGIT}+
FLDS                (f|F|l|L) /*Floating-point and Long Double Suffix*/
ILS                 (u|U|l|L)* /*Integer literal suffix*/

/* 
    "*" = 0+
    "+" = 1+
*/

%%

/* Rules section */
"auto"          { return AUTO; }
"break"         { return BREAK; }
"case"          { return CASE; }
"char"          { return CHAR; }
"const"         { return CONST; }
"continue"      { return CONTINUE; }
"default"       { return DEFAULT; }
"do"            { return DO; }
"double"        { return DOUBLE; }
"else"          { return ELSE; }
"enum"          { return ENUM; }
"extern"        { return EXTERN; }
"float"         { return FLOAT; }
"for"           { return FOR; }
"goto"          { return GOTO; }
"if"            { return IF; }
"int"           { return INT; }
"long"          { return LONG; }
"register"      { return REGISTER; }
"return"        { return RETURN; }
"short"         { return SHORT; }
"signed"        { return SIGNED; }
"sizeof"        { return SIZEOF; }
"static"        { return STATIC; }
"struct"        { return STRUCT; }
"switch"        { return SWITCH; }
"typedef"       { return TYPEDEF; }
"union"         { return UNION; }
"unsigned"      { return UNSIGNED; }
"void"          { return VOID; }
"volatile"      { return VOLATILE; }
"while"         { return WHILE; }


%%

/* C Code section */
int main(void)
{
    yylex(); 
    // code to use lexer
    return 0;
}%%

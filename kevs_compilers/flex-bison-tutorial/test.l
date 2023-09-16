%{
/* C headers, declarations, variables, etc. */
#include <stdio.h>
#include "test.tab.h"

void showError();

%}

/* Regular expressions */

numbers ([0-9])+
alpha   ([a-zA-Z])+

%%
/* Token Specifications */

{alpha}     {sscanf(yytext, "%s", yylval.name); return (STRING);} 
/* input from yytext, output to yylval.name */
/* any kind of input is going into yytext */
/* union in the yacc file = yylval */

{numbers}   {yyval.number = atoi(yytext); return (NUM)}
";"         {return (SEMICOLON);}
.           {showError(); return (OTHER);}


%%

void showError(){
    printf("other input");
}
%{
#include "parser.tab.h"
%}

%%

int|return      { return yytext[0]; }  // A simple way to recognize keywords and some operators for this example.
[0-9]+         { yylval.intVal = atoi(yytext); return DIGIT; }
[a-zA-Z_][a-zA-Z0-9_]* { return IDENTIFIER; }
[ \t\n]        ;   // Skip whitespaces
.              { return yytext[0]; }  // Catch all for other characters

%%

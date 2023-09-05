/*** Definition section ***/
%option noyywrap
%{
    #include <stdio.h>
%}

%%

/*** Rules section ***/
"auto"          { return AUTO; }
"break"         { return BREAK; }


%%

/*** C Code section ***/
int main(void)
{
    yylex();
    return 0;
}%%

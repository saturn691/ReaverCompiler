%{
** C headers, declarations, variables, etc. **
#include <stdio.h>
#include "test.tab.h"

void showError();

%}

** Regular expressions **

numbers ([0-9])+

%%
** Token Specifications **

"hello"     {return (STRING);}
{numbers}   {return (NUM);}
";"         {return (SEMICOLON);}
.           {showError(); return (OTHER);}


%%

void showError(){
    printf("other input");
}
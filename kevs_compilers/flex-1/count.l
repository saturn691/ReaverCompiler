/* Definition Section has variables
which can be accessed inside yylex()
and main() */
%{
int cap_num = 0;
int lines_num = 0;
int char_num = 0;
%}

/* Rule Section */
%%
[A-Z] {printf("%s cap\n", yytext); char_num++; cap_num++;}
.     {printf("%s not cap\n", yytext); char_num++;}
\n    ++lines_num;
end   return 0;
%%
 
/* Code Section*/
int yywrap(){}
int main(){
 
// Explanation:
// yywrap() - wraps the above rule section
/* yyin - takes the file pointer
          which contains the input*/
/* yylex() - this is the main flex function
          which runs the Rule Section*/
// yytext is the text in the buffer
 
// Uncomment the lines below
// to take input from file
// FILE *fp;
// char filename[50];
// printf("Enter the filename: \n");
// scanf("%s",filename);
// fp = fopen(filename,"r");
// yyin = fp;
 
yylex();
printf("\nNumber of Capital letters "
      "in the given input - %d\n", cap_num);
printf("number of lines = %d, number of chars = %d\n",
       lines_num, char_num );
return 0;
}
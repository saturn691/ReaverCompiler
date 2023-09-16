%option noyywrap
%{
    #include <stdio.h>
    // #include "parser.tab.hpp"
    // #include "y.tab.h"
%}

/* https://www.lysator.liu.se/c/ANSI-C-grammar-l.html#MUL-ASSIGN */

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L) /*Floating-point and Long Double Suffix*/
IS			(u|U|l|L)* /*Integer literal suffix*/

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

/* lex syntax missing for all the combinations of the above types*/

"..."			{   return(ELLIPSIS); }
">>="			{   return(RIGHT_ASSIGN); }
"<<="			{   return(LEFT_ASSIGN); }
"+="			{   return(ADD_ASSIGN); }
"-="			{   return(SUB_ASSIGN); }
"*="			{   return(MUL_ASSIGN); }
"/="			{   return(DIV_ASSIGN); }
"%="			{   return(MOD_ASSIGN); }
"&="			{   return(AND_ASSIGN); }
"^="			{   return(XOR_ASSIGN); }
"|="			{   return(OR_ASSIGN); }
">>"			{   return(RIGHT_OP); }
"<<"			{   return(LEFT_OP); }
"++"			{   return(INC_OP); }
"--"			{   return(DEC_OP); }
"->"			{   return(PTR_OP); }
"&&"			{   return(AND_OP); }
"||"			{   return(OR_OP); }
"<="			{   return(LE_OP); }
">="			{   return(GE_OP); }
"=="			{   return(EQ_OP); }
"!="			{   return(NE_OP); }
";"		    	{   return(';'); }
("{"|"<%")		{   return('{'); }
("}"|"%>")		{   return('}'); }
","		    	{   return(','); }
":"			    {   return(':'); }
"="			    {   return('='); }
"("			    {   return('('); }
")"			    {   return(')'); }
("["|"<:")		{   return('['); }
("]"|":>")		{   return(']'); }
"."			    {   return('.'); }
"&"			    {   return('&'); }
"!"			    {   return('!'); }
"~"		    	{   return('~'); }
"-"			    {   return('-'); }
"+"			    {   return('+'); }
"*"			    {   return('*'); }
"/"		    	{   return('/'); }
"%"		    	{   return('%'); }
"<"		    	{   return('<'); }
">"		    	{   return('>'); }
"^"		    	{   return('^'); }
"|"		    	{   return('|'); }
"?"		    	{   return('?'); }

[ \t\v\n\f]		{  }
.			{ /* ignore bad characters */ }

/*Odyssey uses:
    [ \t\r\n] { ; }
with no catch*/

%%

void yyerror (char const *s)
{
  fprintf(stderr, "Lexing error: %s\n", s);
  exit(1);
}

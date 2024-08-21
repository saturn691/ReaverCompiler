%option noyywrap
%x C_COMMENT

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*

%{
#include <stdio.h>
#include "parser.tab.hpp"


%}

%%

"/*"			{ BEGIN(C_COMMENT); }
<C_COMMENT>"*/"		{ BEGIN(INITIAL); }
<C_COMMENT>.		{ ; }

\/\/(.*?)\n		{ ; }

"#define"(.*?)\n	{ /* Ignore #define lines */ }
"#include"(.*?)\n	{ /* Ignore #include lines */ }
"#undef"(.*?)\n		{ /* Ignore #undef lines */ }
"#ifdef"(.*?)\n		{ /* Ignore #ifdef lines */ }
"#ifndef"(.*?)\n	{ /* Ignore #ifndef lines */ }
"#if"(.*?)\n		{ /* Ignore #if lines */ }
"#else"(.*?)\n		{ /* Ignore #else lines */ }
"#elif"(.*?)\n		{ /* Ignore #elif lines */ }
"#endif"(.*?)\n		{ /* Ignore #endif lines */ }
"#error"(.*?)\n		{ /* Ignore #error lines */ }
"#pragma"(.*?)\n	{ /* Ignore #pragma lines */ }

"auto"			{ return(AUTO); }
"break"			{ return(BREAK); }
"case"			{ return(CASE); }
"char"			{ return(CHAR); }
"const"			{ return(CONST); }
"continue"		{ return(CONTINUE); }
"default"		{ return(DEFAULT); }
"do"			{ return(DO); }
"double"		{ return(DOUBLE); }
"else"			{ return(ELSE); }
"enum"			{ return(ENUM); }
"extern"		{ return(EXTERN); }
"float"			{ return(FLOAT); }
"for"			{ return(FOR); }
"goto"			{ return(GOTO); }
"if"			{ return(IF); }
"int"			{ return(INT); }
"long"			{ return(LONG); }
"register"		{ return(REGISTER); }
"return"		{ return(RETURN); }
"short"			{ return(SHORT); }
"signed"		{ return(SIGNED); }
"sizeof"		{ return(SIZEOF); }
"static"		{ return(STATIC); }
"struct"		{ return(STRUCT); }
"switch"		{ return(SWITCH); }
"typedef"		{ return(TYPEDEF); }
"union"			{ return(UNION); }
"unsigned"		{ return(UNSIGNED); }
"void"			{ return(VOID); }
"volatile"		{ return(VOLATILE); }
"while"			{ return(WHILE); }

{L}({L}|{D})*		{ yylval.string = new std::string(yytext); return(IDENTIFIER); }

0[xX]{H}+{IS}?		{ yylval.string = new std::string(yytext); return(CONSTANT); }
0{D}+{IS}?		{ yylval.string = new std::string(yytext); return(CONSTANT); }
{D}+{IS}?		{ yylval.string = new std::string(yytext); return(CONSTANT); }

L?'(\\.|[^\\'])+'	{ yylval.string = new std::string(yytext); return(CHAR_LITERAL); }

{D}+{E}{FS}?		{ yylval.string = new std::string(yytext); return(CONSTANT); }
{D}*"."{D}+({E})?{FS}?	{ yylval.string = new std::string(yytext); return(CONSTANT); }
{D}+"."{D}*({E})?{FS}?	{ yylval.string = new std::string(yytext); return(CONSTANT); }

L?\"(\\.|[^\\"])*\"	{ yylval.string = new std::string(yytext); return(STRING_LITERAL); }

"..."			{ return(ELLIPSIS); }
">>="			{ return(RIGHT_ASSIGN); }
"<<="			{ return(LEFT_ASSIGN); }
"+="			{ return(ADD_ASSIGN); }
"-="			{ return(SUB_ASSIGN); }
"*="			{ return(MUL_ASSIGN); }
"/="			{ return(DIV_ASSIGN); }
"%="			{ return(MOD_ASSIGN); }
"&="			{ return(AND_ASSIGN); }
"^="			{ return(XOR_ASSIGN); }
"|="			{ return(OR_ASSIGN); }
">>"			{ return(RIGHT_OP); }
"<<"			{ return(LEFT_OP); }
"++"			{ return(INC_OP); }
"--"			{ return(DEC_OP); }
"->"			{ return(PTR_OP); }
"&&"			{ return(AND_OP); }
"||"			{ return(OR_OP); }
"<="			{ return(LE_OP); }
">="			{ return(GE_OP); }
"=="			{ return(EQ_OP); }
"!="			{ return(NE_OP); }
";"			{ return(';'); }
("{"|"<%")		{ return('{'); }
("}"|"%>")		{ return('}'); }
","			{ return(','); }
":"			{ return(':'); }
"="			{ return('='); }
"("			{ return('('); }
")"			{ return(')'); }
("["|"<:")		{ return('['); }
("]"|":>")		{ return(']'); }
"."			{ return('.'); }
"&"			{ return('&'); }
"!"			{ return('!'); }
"~"			{ return('~'); }
"-"			{ return('-'); }
"+"			{ return('+'); }
"*"			{ return('*'); }
"/"			{ return('/'); }
"%"			{ return('%'); }
"<"			{ return('<'); }
">"			{ return('>'); }
"^"			{ return('^'); }
"|"			{ return('|'); }
"?"			{ return('?'); }

[ \t\v\n\f]		{ }
.			{ /* ignore bad characters */ }

%%


void yyerror(const char *s)
{
	fprintf(stderr, "Parse error: %s\n", s);
	exit(1);
}

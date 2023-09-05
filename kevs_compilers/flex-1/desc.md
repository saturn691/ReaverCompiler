Tutorial from:
https://www.geeksforgeeks.org/flex-fast-lexical-analyzer-generator/amp/

>> Explains the basics of Flex (Fast Lexical Analyzer Generator)

>> Running flex
    1. flex filename.l
    2. gcc lex.yy.c
    3. ./a.out
    4. Provide input

Notes:
>> .l file provided to flex --> yylex() function auto-gen by flex --> yylex() is expected by parser to call to retrieve tokens from token stream
>> The function yylex() is the main flex function that runs the Rule Section and extension (.l) is the extension used to save the programs.
>> Steps: 
    >> Step 1: An input file describes the lexical analyzer to be generated named lex.l is written in lex language. The lex compiler transforms lex.l to C program, in a file that is always named lex.yy.c. 
    >> Step 2: The C compiler compile lex.yy.c file into an executable file called a.out. 
    >> Step 3: The output file a.out take a stream of input characters and produce a stream of tokens. 
>> Sections:
    1. Definition section
        Declaration of variables, regular definitions, etc. Copied directly to lex.yy.c
    2. Rules section
        Series of rules in the form of "pattern action"
    3. User code section
        C statements and additional functions



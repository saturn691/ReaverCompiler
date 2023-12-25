#!/bin/bash

## NOTE ##
## Don't run this script in this environment, it was moved over from another project ##

# Author : Kevin Lau (@booth-algo)

# Remove old files
rm -f lex.yy.c parser.tab.c parser.tab.h parser.output y.tab.c y.tab.h y.output ast.dot

# Run Bison/Yacc and Flex
yacc -v -d parser.y
if [ $? -ne 0 ]; then
    echo "Yacc/Bison failed!!!!!!!!!!!!!!"
fi

echo "Yacc/Bison succeeded"

lex lexer.l
if [ $? -ne 0 ]; then
    echo "Lex failed!!!!!!!!!!!!!!"
fi

echo "Lex succeeded"

# Compile the program
gcc y.tab.c -lfl -o myprogram
if [ $? -ne 0 ]; then
    echo "GCC compilation failed!!!!!!!!!!!!!!"
fi

echo "GCC compilation succeeded"

# Run the program
./myprogram < input1.c

echo "Done"


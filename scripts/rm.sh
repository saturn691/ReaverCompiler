#!/bin/bash

# Author : Kevin Lau (@booth-algo)

# Remove old files
for file in ../src/lexer.yy.cpp ../src/parser.tab.cpp ../src/parser.tab.hpp ../src/lexer.yy.o ../src/parser.tab.o ../src/parser.output
do
  if [ -e "$file" ]
  then
    rm -f "$file"
    echo "Removed $file"
  else
    echo "File $file does not exist"
  fi
done

echo "Done"

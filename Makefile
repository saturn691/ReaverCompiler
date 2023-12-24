CPPFLAGS += -std=c++20 -W -Wall -g -I include

.PHONY: default

default: bin/c_compiler

src/parser.tab.cpp src/parser.tab.hpp : src/parser.y
	bison -v -d src/parser.y -o src/parser.tab.cpp

src/lexer.yy.cpp : src/lexer.l src/parser.tab.hpp
	flex -o src/lexer.yy.cpp src/lexer.l

bin/c_compiler : src/cli.cpp src/compiler.cpp src/parser.tab.o src/lexer.yy.o
	@mkdir -p bin
	g++ $(CPPFLAGS) -o bin/c_compiler $^

clean :
	rm -rf bin/*
	rm -f src/*.o
	rm -f src/lexer.yy.cpp
	rm -f src/parser.tab.cpp
	rm -f src/parser.tab.hpp
	rm -f src/parser.output

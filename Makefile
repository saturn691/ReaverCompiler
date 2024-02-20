CPPFLAGS += -std=c++20 -W -Wall -Wextra -Wno-unused-parameter -g -I include

HPPFILES := $(shell find include/ -type f -name "*.hpp")
CPPFILES := $(shell find src/ -type f -name "*.cpp")
OFILES := $(CPPFILES:.cpp=.o)

.PHONY: default

default: bin/c_compiler

src/parser.tab.cpp src/parser.tab.hpp : src/parser.y $(HPPFILES)
	bison -v -d src/parser.y -o src/parser.tab.cpp

src/lexer.yy.cpp : src/lexer.l src/parser.tab.hpp
	flex -o src/lexer.yy.cpp src/lexer.l

%.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

bin/c_compiler : $(OFILES) src/parser.tab.o src/lexer.yy.o
	@mkdir -p bin
	g++ $(CPPFLAGS) -o bin/c_compiler $^

# Add dependency of object files on source files
$(OFILES): $(CPPFILES) $(HPPFILES)

clean :
	rm -rf bin/*
	rm -f src/*.o
	rm -f src/ast/*.o
	rm -f src/lexer.yy.cpp
	rm -f src/parser.tab.cpp
	rm -f src/parser.tab.hpp
	rm -f src/parser.output

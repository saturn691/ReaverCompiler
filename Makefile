CPPFLAGS += -std=c++20 -W -Wall -Wextra -g -I include
FLAGS = $(CPPFLAGS) -Werror

HPPFILES := $(shell find include/ -type f -name "*.hpp")
CPPFILES := $(shell find src/ -type f -name "*.cpp")
OFILES := $(CPPFILES:src/%.cpp=build/%.o)

.PHONY: default clean

default: bin/c_compiler

src/parser.tab.cpp src/parser.tab.hpp : src/parser.y $(HPPFILES)
	bison -v -d src/parser.y -o src/parser.tab.cpp

src/lexer.yy.cpp : src/lexer.l src/parser.tab.hpp
	flex -o src/lexer.yy.cpp src/lexer.l

build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	g++ $(CPPFLAGS) -c $< -o $@

bin/c_compiler : $(OFILES) build/parser.tab.o build/lexer.yy.o
	@mkdir -p bin
	g++ $(FLAGS) -o bin/c_compiler $^

# Add dependency of object files on source files
$(OFILES): $(HPPFILES)

clean :
	rm -rf bin/*
	rm -rf build/*
	rm -f src/lexer.yy.cpp
	rm -f src/parser.tab.cpp
	rm -f src/parser.tab.hpp
	rm -f src/parser.output

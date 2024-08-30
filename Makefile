CPPFLAGS += -std=c++20 -W -Wall -Wextra -g -I include
FLAGS = $(CPPFLAGS) -Werror

CPPFILES := $(wildcard src/ast/models/*.cpp)
CPPFILES += $(wildcard src/ast/utils/*.cpp)
CPPFILES += $(wildcard src/*.cpp)
CPPFILES += $(shell find src/ir -type f -name '*.cpp')
DEPENDENCIES := $(patsubst src/%.cpp,build/%.d,$(CPPFILES))
OFILES := $(patsubst src/%.cpp,build/%.o,$(CPPFILES))
OFILES += build/parser.tab.o build/lexer.yy.o

.PHONY: default clean

default: bin/c_compiler

bin/c_compiler : $(OFILES)
	@mkdir -p bin
	clang++ $(FLAGS) -o $@ $^

-include $(DEPENDENCIES)

build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	clang++ $(CPPFLAGS) -MMD -MP -c $< -o $@

build/parser.tab.cpp build/parser.tab.hpp : src/parser.y
	bison -v -d src/parser.y -o build/parser.tab.cpp

build/lexer.yy.cpp : src/lexer.l build/parser.tab.hpp
	flex -o build/lexer.yy.cpp src/lexer.l

clean :
	rm -rf bin/*
	rm -rf build/*

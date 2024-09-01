CPPFLAGS += -std=c++20 -W -Wall -Wextra -g -I include
CPPFLAGS += $(shell llvm-config-18 --cxxflags)
LLVM_LDFLAGS := $(shell llvm-config-18 --ldflags --libs all)

CPPFILES := $(wildcard src/ast/models/*.cpp)
CPPFILES += $(wildcard src/ast/utils/*.cpp)
CPPFILES += $(wildcard src/codegen/*.cpp)
CPPFILES += $(wildcard src/*.cpp)
CPPFILES += $(shell find src/ir -type f -name '*.cpp')
DEPENDENCIES := $(patsubst src/%.cpp,build/%.d,$(CPPFILES))
OFILES := $(patsubst src/%.cpp,build/%.o,$(CPPFILES))
OFILES += build/parser.tab.o build/lexer.yy.o

.PHONY: default clean install

default: bin/c_compiler

bin/c_compiler : $(OFILES)
	@mkdir -p bin
	clang++ $(CPPFLAGS) $(LLVM_CXXFLAGS) -Werror -o $@ $^ $(LLVM_LDFLAGS)

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

install:
	@echo "Installing dependencies..."
	bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
	@echo "Done!"

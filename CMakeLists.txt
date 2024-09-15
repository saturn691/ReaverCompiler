cmake_minimum_required(VERSION 3.12)

project(rcc
  VERSION 2.0
  DESCRIPTION "C90 Compiler"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate the file for clangd (the LSP)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Mode
set(CMAKE_BUILD_TYPE Debug)

# LLVM
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
llvm_map_components_to_libnames(llvm_libs all)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Parser/Lexer
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
BISON_TARGET(Parser src/parser.y
  ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp
  COMPILE_FLAGS "-v"
)
FLEX_TARGET(Lexer src/lexer.l
  ${CMAKE_CURRENT_BINARY_DIR}/lexer.yy.cpp
)

# Ty
file(GLOB_RECURSE TY_FILES src/ty/*.cpp)
add_library(ty STATIC ${TY_FILES})

# Codegen
file(GLOB_RECURSE CODEGEN_FILES src/codegen/*.cpp)
add_library(codegen STATIC ${CODEGEN_FILES})
target_link_libraries(codegen PUBLIC LLVM ir)

# IR
file(GLOB_RECURSE IR_FILES src/ir/*.cpp)
add_library(ir STATIC ${IR_FILES})

# AST
file(GLOB_RECURSE AST_FILES src/ast/*.cpp)
add_library(ast STATIC ${AST_FILES})
target_link_libraries(ast PUBLIC ir)

# rcc
add_executable(rcc
  src/compiler.cpp
  src/cli.cpp
  ${BISON_Parser_OUTPUTS}
  ${FLEX_Lexer_OUTPUTS}
)

# Affects the whole directory - allows everything to see include/
include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(rcc PRIVATE ir ast codegen ty LLVM)

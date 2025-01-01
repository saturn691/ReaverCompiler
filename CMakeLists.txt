cmake_minimum_required(VERSION 3.20)

# Set before project()
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

project(rcc
    VERSION 2.0
    DESCRIPTION "C99 Compiler"
    LANGUAGES C CXX
)
    
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate the file for clangd (the LSP)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Mode
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -fstandalone-debug")
set(CMAKE_FIND_PACKAGE_SORT_ORDER NATURAL)
set(CMAKE_FIND_PACKAGE_SORT_DIRECTION DEC)

# CLI (main.cpp)
include(FetchContent)
FetchContent_Declare(
    cli11_proj
    QUIET
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.3.2
)
FetchContent_MakeAvailable(cli11_proj)

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

# AST
file(GLOB_RECURSE AST_FILES src/AST/*.cpp)
add_library(AST STATIC ${AST_FILES})

# CodeGen
file(GLOB_RECURSE CODEGEN_FILES src/CodeGen/*.cpp)
add_library(CodeGen STATIC ${CODEGEN_FILES})

# rcc
add_executable(rcc
    src/main.cpp
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

# Affects the whole directory - allows everything to see include/
include_directories(PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(rcc PRIVATE AST CodeGen LLVM CLI11::CLI11)
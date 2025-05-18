cmake_minimum_required(VERSION 3.20)

# Set before project()
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

project(rcc
    VERSION 2.0
    DESCRIPTION "C99 Compiler"
    HOMEPAGE_URL "https://github.com/saturn691/ReaverCompiler"
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

# CCache
find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

#===---------------------------- Dependencies ----------------------------===#

include(FetchContent)

# Boost
set(BOOST_INCLUDE_LIBRARIES filesystem)
set(BOOST_ENABLE_CMAKE ON)
FetchContent_Declare(
    boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.88.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(boost)


# CLI11
FetchContent_Declare(
    cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.3.2
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(cli11)

# LLVM
set(LLVM_ENABLE_PROJECTS    ""              CACHE STRING "" FORCE)
set(LLVM_TARGETS_TO_BUILD   "AArch64;X86"   CACHE STRING "" FORCE)
set(LLVM_INCLUDE_TESTS      OFF             CACHE BOOL   "" FORCE)
FetchContent_Declare(
    llvm_project
    GIT_REPOSITORY https://github.com/llvm/llvm-project.git
    GIT_TAG llvmorg-20.1.5
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(llvm_project)
add_subdirectory(
    ${llvm_project_SOURCE_DIR}/llvm
    ${llvm_project_BINARY_DIR}/llvm
    EXCLUDE_FROM_ALL
)
llvm_map_components_to_libnames(llvm_libs core support passes X86 AArch64)

# GTest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.17.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(googletest)

#===------------------------------ Program -------------------------------===#

# Parser/Lexer
find_package(BISON 3.0 REQUIRED)
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
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${llvm_project_SOURCE_DIR}/llvm/include
    ${llvm_project_BINARY_DIR}/llvm/include
)
target_link_libraries(rcc PRIVATE 
    AST
    CodeGen
    ${llvm_libs}
    CLI11::CLI11
    Boost::filesystem
)

#===-------------------------------- Tests -------------------------------===#

enable_testing()
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_executable(abi_test unittests/CodeGen/ABITest.cpp)
target_link_libraries(abi_test PRIVATE GTest::gtest_main CodeGen ${llvm_libs})

include(GoogleTest)
gtest_discover_tests(abi_test)

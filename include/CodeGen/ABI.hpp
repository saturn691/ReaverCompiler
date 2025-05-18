#pragma once

#include "AST/Type.hpp"

#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

/**
 * Struct field alignment and padding
 * Function calling conventions
 * Parameter passing rules
 * Return value handling
 * Stack frame layout
 * Register allocation preferences
 */

namespace CodeGen
{

class ABI
{
public:
    virtual ~ABI() = default;

    struct FunctionParamsInfo
    {
        llvm::Type *retType;
        std::vector<std::vector<llvm::Type *>> paramTypes;
        bool structReturnInMemory;
    };

    virtual FunctionParamsInfo getFunctionParams(
        llvm::Type *retType,
        std::vector<llvm::Type *> &paramTypes) const = 0;
    virtual llvm::FunctionType *getFunctionType(
        llvm::Type *retType,
        std::vector<llvm::Type *> &paramTypes) const = 0;
    virtual std::vector<llvm::Type *> getParamType(llvm::Type *type) const = 0;
    virtual llvm::Align getTypeAlign(llvm::Type *type) const = 0;
    virtual unsigned getTypeSize(AST::Types type) const = 0;
};

class X86_64ABI : public ABI
{
public:
    enum class ArgClass
    {
        INTEGER, // Integral types that fit in one of the GP registers
        SSE,     // Types that fit into a vector register
        SSEUP,   // Tail part of SSE
        X87,     // 80-bit (rarely used in modern code), returned via x87 FPU
        X87UP,   // Tail part of X87
        COMPLEX_X87, // Complex long double
        MEMORY,      // Passed by memory via the stack
        NO_CLASS     // Types that don't fit into any of the above
    };

    struct ArgClassInfo
    {
        ArgClass cls = ArgClass::NO_CLASS;
        unsigned size = 0;
        unsigned align = 0;
        bool multiple = false;
    };
    using ArgClasses = std::vector<ArgClassInfo>;

    X86_64ABI(llvm::Module &module);

    FunctionParamsInfo getFunctionParams(
        llvm::Type *retType,
        std::vector<llvm::Type *> &paramTypes) const override;
    llvm::FunctionType *getFunctionType(
        llvm::Type *retType,
        std::vector<llvm::Type *> &paramTypes) const override;
    std::vector<llvm::Type *> getParamType(llvm::Type *type) const override;
    llvm::Align getTypeAlign(llvm::Type *type) const override;
    unsigned getTypeSize(AST::Types type) const override;

private:
    ArgClasses getArgClassification(llvm::Type *type) const;
    ArgClassInfo mergeClassifications(ArgClassInfo lhs, ArgClassInfo rhs) const;
    llvm::Module &module_;
};
} // namespace CodeGen
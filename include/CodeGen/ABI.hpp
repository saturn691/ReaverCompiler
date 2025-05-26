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
    virtual bool useByVal() const = 0;
};


} // namespace CodeGen
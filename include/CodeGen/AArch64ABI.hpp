#pragma once

#include "CodeGen/ABI.hpp"

namespace CodeGen
{

class AArch64ABI : public ABI
{
public:
    AArch64ABI(llvm::Module &module);

    FunctionParamsInfo getFunctionParams(
        llvm::Type *retType,
        std::vector<llvm::Type *> &paramTypes) const override;
    llvm::FunctionType *getFunctionType(
        llvm::Type *retType,
        std::vector<llvm::Type *> &paramTypes) const override;
    std::vector<llvm::Type *> getParamType(llvm::Type *type) const override;
    llvm::Align getTypeAlign(llvm::Type *type) const override;
    unsigned getTypeSize(AST::Types type) const override;

    bool useByVal() const override
    {
        return false;
    }

private:
    llvm::Module &module_;
};
} // namespace CodeGen

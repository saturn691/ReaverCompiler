#pragma once

#include <map>
#include <memory>
#include <ostream>
#include <string>

#include <ir/ir_visitor.hpp>
#include <ir/models/ir_ir.hpp>

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

namespace codegen
{
class IRCodegen : public ir::Visitor
{
public:
    IRCodegen(std::unique_ptr<ir::IR> ir);
    ~IRCodegen() = default;

    void codegen();
    void codegen(const ir::BasicBlock &block);
    void print(std::ostream &dst) const;
    void optimize();

    llvm::Function *codegen(const ir::FunctionHeader &header);
    llvm::Function *codegen(const ir::Function &function);

    llvm::Value *codegen(const ir::BinaryOp &expr) override;
    llvm::Value *codegen(const ir::Cast &expr) override;
    llvm::Value *codegen(const ir::Constant &expr) override;
    llvm::Value *codegen(const ir::Use &expr) override;
    llvm::Value *codegen(const ir::Assign &stmt) override;

    std::string codegen(const ir::Declaration &stmt) override;

    void codegen(const ir::Return &stmt) override;
    void codegen(const ir::Goto &stmt) override;
    void codegen(const ir::SwitchInt &stmt) override;

private:
    llvm::Type *to_llvm_type(const ir::Type &type);
    llvm::Type *to_llvm_type(const ty::Types &type);

    std::unique_ptr<ir::IR> ir;
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;

    std::vector<llvm::Function *> functions;
    std::vector<llvm::BasicBlock *> bbs;
    std::map<std::string, llvm::AllocaInst *> named_values;
};
} // namespace codegen

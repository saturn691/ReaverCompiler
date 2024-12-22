#pragma once

#include <iostream>

#include "AST/Visitor.hpp"
#include "CodeGen/TypeChecker.hpp"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

using namespace AST;

namespace CodeGen
{

class CodeGenModule : public Visitor
{
public:
    CodeGenModule(std::ostream &os, TypeMap &typeMap);
    void print();
    void optimize();

    // Declarations
    void visit(const DeclNode &node) override;
    void visit(const FnDecl &node) override;
    void visit(const FnDef &node) override;
    void visit(const ParamDecl &node) override;
    void visit(const ParamList &node) override;
    void visit(const TranslationUnit &node) override;

    // Expressions
    void visit(const BinaryOp &node) override;
    void visit(const Constant &node) override;
    void visit(const Identifier &node) override;

    // Statements
    void visit(const BlockItemList &node) override;
    void visit(const CompoundStmt &node) override;
    void visit(const Return &node) override;

    // Types
    void visit(const BasicType &node) override;

private:
    std::ostream &os_;
    TypeMap &typeMap_;

    std::unique_ptr<llvm::LLVMContext> context_;
    std::unique_ptr<llvm::IRBuilder<>> builder_;
    std::unique_ptr<llvm::Module> module_;

    llvm::Value *currentValue_ = nullptr;

    llvm::Function *getCurrentFunction();
    llvm::Type *getLLVMType(const BaseType *type);
};
} // namespace CodeGen
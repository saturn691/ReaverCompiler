#pragma once

#include <iostream>
#include <stack>
#include <unordered_map>

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
    void visit(const ArrayDecl &node) override;
    void visit(const BasicTypeDecl &node) override;
    void visit(const DeclNode &node) override;
    void visit(const DefinedTypeDecl &node) override;
    void visit(const Enum &node) override;
    void visit(const EnumMember &node) override;
    void visit(const EnumMemberList &node) override;
    void visit(const FnDecl &node) override;
    void visit(const FnDef &node) override;
    void visit(const InitDecl &node) override;
    void visit(const InitDeclList &node) override;
    void visit(const ParamDecl &node) override;
    void visit(const ParamList &node) override;
    void visit(const PtrDecl &node) override;
    void visit(const PtrNode &node) override;
    void visit(const Struct &node) override;
    void visit(const StructDecl &node) override;
    void visit(const StructDeclList &node) override;
    void visit(const StructMember &node) override;
    void visit(const StructMemberList &node) override;
    void visit(const TranslationUnit &node) override;
    void visit(const Typedef &node) override;

    // Expressions (should not be called directly)
    void visit(const ArrayAccess &node) override;
    void visit(const Assignment &node) override;
    void visit(const ArgExprList &node) override;
    void visit(const BinaryOp &node) override;
    void visit(const Constant &node) override;
    void visit(const FnCall &node) override;
    void visit(const Identifier &node) override;
    void visit(const Paren &node) override;
    void visit(const SizeOf &node) override;
    void visit(const StringLiteral &node) override;
    void visit(const StructAccess &node) override;
    void visit(const StructPtrAccess &node) override;
    void visit(const UnaryOp &node) override;

    // Statements
    void visit(const BlockItemList &node) override;
    void visit(const Break &node) override;
    void visit(const Case &node) override;
    void visit(const CompoundStmt &node) override;
    void visit(const Continue &node) override;
    void visit(const ExprStmt &node) override;
    void visit(const For &node) override;
    void visit(const IfElse &node) override;
    void visit(const Return &node) override;
    void visit(const Switch &node) override;
    void visit(const While &node) override;

private:
    std::ostream &os_;
    TypeMap &typeMap_;

    std::unique_ptr<llvm::LLVMContext> context_;
    std::unique_ptr<llvm::IRBuilder<>> builder_;
    std::unique_ptr<llvm::Module> module_;

    enum class ValueCategory
    {
        LVALUE,
        RVALUE,
        FN_DESIGNATOR
    };

    // Contextual information (unfortunately). Use the guard for safety.
    // Used as a "return value" for the visitor
    llvm::Value *currentValue_ = nullptr;
    llvm::SwitchInst *currentSwitch_ = nullptr;
    ValueCategory valueCategory_ = ValueCategory::RVALUE;
    bool isGlobal_ = true;

    using SymbolTable =
        std::vector<std::unordered_map<std::string, llvm::AllocaInst *>>;
    using ConstTable =
        std::vector<std::unordered_map<std::string, llvm::Constant *>>;

    SymbolTable symbolTable_;
    ConstTable constTable_;
    std::stack<llvm::BasicBlock *> breakStack_;
    std::stack<llvm::BasicBlock *> continueStack_;

    llvm::Function *getCurrentFunction();
    llvm::Type *getLLVMType(const BaseNode *node);
    llvm::Type *getLLVMType(const BaseType *type);
    llvm::Type *getPointerElementType(const BaseNode *node);
    llvm::Value *visitAsLValue(const Expr &node);
    llvm::Value *visitAsRValue(const Expr &node);
    llvm::Function *visitAsFnDesignator(const Expr &expr);

    void symbolTablePush(std::string id, llvm::AllocaInst *alloca);
    llvm::AllocaInst *symbolTableLookup(std::string id) const;

    void constTablePush(std::string id, llvm::Constant *constant);
    llvm::Constant *constTableLookup(std::string id) const;

    void pushScope();
    void popScope();
};

/**
 * Helper class for passing information down the AST.
 */
template <typename T>
class ScopeGuard
{
public:
    ScopeGuard(T &ref, const T &newValue) : ref_(ref), oldValue_(ref)
    {
        ref = newValue;
    }
    ~ScopeGuard()
    {
        ref_ = oldValue_;
    }

private:
    T &ref_;
    T oldValue_;
};

template <typename U>
class ScopeGuard<std::stack<U>>
{
public:
    ScopeGuard(std::stack<U> &ref, U newValue) : ref_(ref)
    {
        ref.push(std::move(newValue));
    }
    ~ScopeGuard()
    {
        ref_.pop();
    }

private:
    std::stack<U> &ref_;
};

} // namespace CodeGen
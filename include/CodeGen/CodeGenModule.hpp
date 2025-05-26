#pragma once

#include <iostream>
#include <stack>
#include <unordered_map>

#include "AST/Visitor.hpp"
#include "CodeGen/AArch64ABI.hpp"
#include "CodeGen/TypeChecker.hpp"
#include "CodeGen/X86_64ABI.hpp"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Target/TargetMachine.h"

using namespace AST;

namespace CodeGen
{
using Symbol = std::variant<
    llvm::AllocaInst *,
    llvm::Argument *,
    llvm::Constant *,
    llvm::GlobalVariable *,
    std::monostate>;
using SymbolTable = std::vector<std::unordered_map<std::string, Symbol>>;

class CodeGenModule : public Visitor
{
public:
    CodeGenModule(
        std::string sourceFile,
        std::string outputFile,
        TypeMap &typeMap,
        std::string targetTriple);
    void emitLLVM();
    void emitObject();
    void optimize();

    // Declarations
    void visit(const AbstractArrayDecl &node) override;
    void visit(const AbstractTypeDecl &node) override;
    void visit(const ArrayDecl &node) override;
    void visit(const BasicTypeDecl &node) override;
    void visit(const CompoundTypeDecl &node) override;
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
    void visit(const TypeModifier &node) override;

    // Expressions (should not be called directly)
    void visit(const ArrayAccess &node) override;
    void visit(const Assignment &node) override;
    void visit(const ArgExprList &node) override;
    void visit(const BinaryOp &node) override;
    void visitLogicalOp(const BinaryOp &node);
    void visitPtrOp(const BinaryOp &node);
    void visit(const Cast &node) override;
    void visit(const Constant &node) override;
    void visit(const FnCall &node) override;
    void visit(const Identifier &node) override;
    void visit(const Init &node) override;
    void visit(const InitList &node) override;
    void visitRecursiveStore(
        const InitList &node,
        std::vector<llvm::Value *> indices);
    llvm::Constant *visitRecursiveConst(const InitList &node);
    void visit(const Paren &node) override;
    void visit(const SizeOf &node) override;
    void visit(const StringLiteral &node) override;
    void visit(const StructAccess &node) override;
    void visit(const StructPtrAccess &node) override;
    void visit(const TernaryOp &node) override;
    void visit(const UnaryOp &node) override;

    // Statements
    void visit(const BlockItemList &node) override;
    void visit(const Break &node) override;
    void visit(const Case &node) override;
    void visit(const CompoundStmt &node) override;
    void visit(const Continue &node) override;
    void visit(const DoWhile &node) override;
    void visit(const ExprStmt &node) override;
    void visit(const For &node) override;
    void visit(const IfElse &node) override;
    void visit(const Return &node) override;
    void visit(const Switch &node) override;
    void visit(const While &node) override;

private:
    enum class ValueCategory
    {
        LVALUE,
        RVALUE,
        FN_DESIGNATOR
    };

    std::string outputFile_;
    TypeMap &typeMap_;

    std::unique_ptr<llvm::LLVMContext> context_;
    std::unique_ptr<llvm::IRBuilder<>> builder_;
    std::unique_ptr<llvm::Module> module_;
    std::unique_ptr<ABI> abi_;
    llvm::TargetMachine *targetMachine_;

    // Contextual information (unfortunately). Use the guard for safety.
    // Used as a "return value" for the visitor
    llvm::Value *currentValue_ = nullptr;                 // For Expr
    ValueCategory valueCategory_ = ValueCategory::RVALUE; // For Expr
    llvm::Value *currentStore_ = nullptr;                 // For InitDecl
    bool isGlobal_ = true;                                // For InitDecl
    const BaseType *currentExpectedType_ = nullptr;       // For InitDecl
    llvm::Function *currentFunction_ = nullptr; // For FnDecl/ParamList
    llvm::SwitchInst *currentSwitch_ = nullptr; // For Switch/Case
    SymbolTable symbolTable_;                   // For Decl
    // For Break/While/For/Do-While/Switch
    std::stack<llvm::BasicBlock *> breakStack_;
    // For Continue/While/For/Do-While
    std::stack<llvm::BasicBlock *> continueStack_;
    std::unordered_map<std::string, std::vector<size_t>> structIDs_;

    llvm::AllocaInst *
    createAlignedAlloca(llvm::Type *type, const llvm::Twine &name = "");
    llvm::GlobalVariable *createAlignedGlobalVariable(
        llvm::Module &M,
        llvm::Type *Ty,
        bool isConstant,
        llvm::GlobalVariable::LinkageTypes Linkage,
        llvm::Constant *Initializer,
        const llvm::Twine &Name = "",
        llvm::GlobalVariable *InsertBefore = nullptr,
        llvm::GlobalVariable::ThreadLocalMode ThreadLocalMode =
            llvm::GlobalVariable::NotThreadLocal,
        std::optional<unsigned> AddressSpace = std::nullopt,
        bool isExternallyInitialized = false);
    llvm::Function *createFunction(
        const FnType *fnType,
        llvm::GlobalValue::LinkageTypes linkage,
        const std::string &name);

    llvm::Align getAlign(llvm::Type *type) const;
    llvm::Function *getCurrentFunction() const;
    std::string getLocalStaticName(const std::string &name) const;
    llvm::Type *getLLVMType(const BaseNode *node);
    llvm::Type *getLLVMType(const BaseType *type);
    llvm::Type *getLLVMType(Types ty);
    std::vector<llvm::Type *> getParamTypes(const FnType *fnType);
    llvm::Type *getPointerElementType(const BaseNode *node);
    llvm::Value *visitAsLValue(const Expr &node);
    llvm::Value *visitAsRValue(const Expr &node);
    llvm::Value *
    visitAsCastedRValue(const Expr &node, const BaseType *expectedType);
    llvm::Function *visitAsFnDesignator(const Expr &node);
    llvm::Constant *
    visitAsConstant(const Expr &node, const BaseType *expectedType);
    llvm::Value *visitAsStore(
        const Expr &node,
        llvm::Value *storeVal,
        const BaseType *expectedType);
    void symbolTablePush(std::string id, Symbol symbol);
    Symbol symbolTableLookup(std::string id) const;

    void pushScope();
    void popScope();

    // These generate code in the LLVM IR
    llvm::Value *isNotZero(llvm::Value *val);
    Types getArithmeticConversionType(const BaseType *lhs, const BaseType *rhs);
    llvm::Value *runConversions(
        const BaseType *lhs,
        const BaseType *rhs,
        llvm::Value *lhsVal);
    llvm::Value *runUsualArithmeticConversions(
        const BaseType *lhs,
        const BaseType *rhs,
        llvm::Value *lhsVal);
    void runIntegerPromotions(const BaseType *type, llvm::Value *&val);
    llvm::Value *runCast(
        llvm::Value *val,
        const BaseType *initialType,
        const BaseType *expectedType);
};

} // namespace CodeGen
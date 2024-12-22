#include "CodeGen/CodeGenModule.hpp"
#include "AST/Decl.hpp"
#include "AST/Expr.hpp"
#include "AST/Stmt.hpp"
#include "AST/Type.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar/Reassociate.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>

#include <iostream>

namespace CodeGen
{
/******************************************************************************
 *                          Public functions                                  *
 *****************************************************************************/

CodeGenModule::CodeGenModule(std::ostream &os, TypeMap &typeMap)
    : os_(os), typeMap_(typeMap),
      context_(std::make_unique<llvm::LLVMContext>()),
      builder_(std::make_unique<llvm::IRBuilder<>>(*context_)),
      module_(std::make_unique<llvm::Module>("Module", *context_))
{
}

void CodeGenModule::print()
{
    llvm::raw_os_ostream oss(os_);
    module_->print(oss, nullptr);
}

void CodeGenModule::optimize()
{
    // Analysis managers - must be in order
    auto lam = std::make_unique<llvm::LoopAnalysisManager>();
    auto fam = std::make_unique<llvm::FunctionAnalysisManager>();
    auto cgam = std::make_unique<llvm::CGSCCAnalysisManager>();
    auto mam = std::make_unique<llvm::ModuleAnalysisManager>();

    auto fpm = std::make_unique<llvm::FunctionPassManager>();

    // Peephole optimizations and bit-twiddling
    fpm->addPass(llvm::InstCombinePass());
    // Reassociate expressions
    fpm->addPass(llvm::ReassociatePass());
    // Eliminate common subexpressions
    fpm->addPass(llvm::GVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc.)
    fpm->addPass(llvm::SimplifyCFGPass());

    // Customisation options available in the PassBuilder
    auto pb = llvm::PassBuilder();
    pb.registerModuleAnalyses(*mam);
    pb.registerCGSCCAnalyses(*cgam);
    pb.registerFunctionAnalyses(*fam);
    pb.registerLoopAnalyses(*lam);
    pb.crossRegisterProxies(*lam, *fam, *cgam, *mam);

    auto mpm = pb.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O2);

    mpm.run(*module_.get(), *mam);
}

/******************************************************************************
 *                          Declarations                                      *
 *****************************************************************************/

void CodeGenModule::visit(const DeclNode &node)
{
}

void CodeGenModule::visit(const FnDecl &node)
{
    // Label the function arguments
    node.params_->accept(*this);
}

void CodeGenModule::visit(const FnDef &node)
{
    // Safe to do... this was checked in the TypeChecker
    const FnType *type = dynamic_cast<const FnType *>(typeMap_[&node].get());
    llvm::Type *retType = getLLVMType(type->retType_.get());
    std::vector<llvm::Type *> paramTypes;

    for (const auto &p : type->params_->types_)
    {
        paramTypes.push_back(getLLVMType(p.get()));
    }

    llvm::FunctionType *fnType =
        llvm::FunctionType::get(retType, paramTypes, false);

    llvm::Function *fn = llvm::Function::Create(
        fnType,
        llvm::Function::ExternalLinkage,
        node.decl_->getID(),
        module_.get());

    llvm::BasicBlock *bb = llvm::BasicBlock::Create(*context_, "entry", fn);
    builder_->SetInsertPoint(bb);

    // Label the function arguments
    node.decl_->accept(*this);

    // Generate the function body
    node.body_->accept(*this);
}

void CodeGenModule::visit(const ParamDecl &node)
{
    // Taken care of in ParamList
}

void CodeGenModule::visit(const ParamList &node)
{
    // Label the function arguments
    llvm::Function *fn = getCurrentFunction();
    unsigned int i = 0;

    for (auto &arg : fn->args())
    {
        arg.setName(std::get<Ptr<ParamDecl>>(node.nodes_[i])->getID());
        i++;
    }
}

void CodeGenModule::visit(const TranslationUnit &node)
{
    for (const auto &decl : node.nodes_)
    {
        std::visit([this](const auto &decl) { decl->accept(*this); }, decl);
    }
}

/******************************************************************************
 *                          Expressions                                       *
 *****************************************************************************/

void CodeGenModule::visit(const BinaryOp &node)
{
}

void CodeGenModule::visit(const Constant &node)
{
    llvm::Type *type = getLLVMType(typeMap_[&node].get());
    currentValue_ = llvm::ConstantInt::get(type, std::stoi(node.value_));
}

void CodeGenModule::visit(const Identifier &node)
{
}

/******************************************************************************
 *                          Statements                                        *
 *****************************************************************************/

void CodeGenModule::visit(const BlockItemList &node)
{
    for (const auto &item : node.nodes_)
    {
        std::visit([this](const auto &item) { item->accept(*this); }, item);
    }
}

void CodeGenModule::visit(const CompoundStmt &node)
{
    node.nodes_->accept(*this);
}

void CodeGenModule::visit(const Return &node)
{
    if (node.expr_.has_value())
    {
        node.expr_->get()->accept(*this);
    }

    if (currentValue_)
    {
        builder_->CreateRet(currentValue_);
    }
    else
    {
        builder_->CreateRetVoid();
    }
}

/******************************************************************************
 *                          Types                                             *
 *****************************************************************************/

void CodeGenModule::visit(const BasicType &node)
{
}

/******************************************************************************
 *                          Private methods                                   *
 *****************************************************************************/

llvm::Function *CodeGenModule::getCurrentFunction()
{
    return builder_->GetInsertBlock()->getParent();
}

llvm::Type *CodeGenModule::getLLVMType(const BaseType *type)
{
    // Basic types
    if (auto basicType = dynamic_cast<const BasicType *>(type))
    {
        switch (basicType->type_)
        {
        case Types::VOID:
            return llvm::Type::getVoidTy(*context_);
        case Types::CHAR:
            return llvm::Type::getInt8Ty(*context_);
        case Types::INT:
            return llvm::Type::getInt32Ty(*context_);
        case Types::FLOAT:
            return llvm::Type::getFloatTy(*context_);
        case Types::DOUBLE:
            return llvm::Type::getDoubleTy(*context_);
        default:
            return nullptr;
        }
    }

    throw std::runtime_error("Unknown type");
}

} // namespace CodeGen
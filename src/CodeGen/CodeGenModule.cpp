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
    // Allocate memory for the variable
    node.initDeclList_->accept(*this);
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
    symbolTable_.clear();
    for (auto &arg : fn->args())
    {
        std::string paramName = arg.getName().str();
        symbolTable_[paramName] =
            builder_->CreateAlloca(arg.getType(), nullptr, arg.getName());
        builder_->CreateStore(&arg, symbolTable_[paramName]);
    }

    // Generate the function body
    isGlobal_ = false;
    node.body_->accept(*this);
    isGlobal_ = true;
}

void CodeGenModule::visit(const InitDecl &node)
{
    if (isGlobal_)
    {
        llvm::Type *type = getLLVMType(typeMap_[&node].get());
        if (type->isFunctionTy())
        {
            // Function declaration
            llvm::Function *fn = llvm::Function::Create(
                static_cast<llvm::FunctionType *>(type),
                llvm::Function::ExternalLinkage,
                node.getID(),
                module_.get());
        }
    }
    else
    {
        // Allocate memory for the variable
        llvm::Type *type = getLLVMType(typeMap_[&node].get());
        llvm::AllocaInst *alloca =
            builder_->CreateAlloca(type, nullptr, node.getID());
        symbolTable_[node.getID()] = alloca;

        if (node.expr_)
        {
            node.expr_->accept(*this);
            builder_->CreateStore(currentValue_, alloca);
        }
    }
}

void CodeGenModule::visit(const InitDeclList &node)
{
    for (const auto &initDecl : node.nodes_)
    {
        std::visit(
            [this](const auto &initDecl) { initDecl->accept(*this); },
            initDecl);
    }
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
 *                  Must separate LValue and RValue handling                  *
 *****************************************************************************/

void CodeGenModule::visit(const Assignment &node)
{
    if (valueCategory_ == ValueCategory::LVALUE)
    {
        throw std::runtime_error("Assignment to LValue not supported");
    }

    using Op = Assignment::Op;

    llvm::Value *lhs = visitAsLValue(*node.lhs_);
    llvm::Value *rhs = visitAsRValue(*node.rhs_);

    switch (node.op_)
    {
    case Op::ASSIGN:
        currentValue_ = builder_->CreateStore(rhs, lhs);
        break;
    }
}

void CodeGenModule::visit(const ArgExprList &node)
{
    // Intentionally left blank
}

void CodeGenModule::visit(const BinaryOp &node)
{
    if (valueCategory_ == ValueCategory::LVALUE)
    {
        throw std::runtime_error("BinaryOp to LValue not supported");
    }

    using Op = BinaryOp::Op;

    llvm::Value *lhs = visitAsRValue(*node.lhs_);
    llvm::Value *rhs = visitAsRValue(*node.rhs_);

    bool isFloat = lhs->getType()->isFloatingPointTy();

    // TODO types
    switch (node.op_)
    {
    case Op::ADD:
        currentValue_ = (isFloat) ? builder_->CreateFAdd(lhs, rhs, "add")
                                  : builder_->CreateAdd(lhs, rhs, "add");
        break;
    case Op::SUB:
        currentValue_ = (isFloat) ? builder_->CreateFSub(lhs, rhs, "sub")
                                  : builder_->CreateSub(lhs, rhs, "sub");
        break;
    case Op::MUL:
        currentValue_ = (isFloat) ? builder_->CreateFMul(lhs, rhs, "mul")
                                  : builder_->CreateMul(lhs, rhs, "mul");
        break;
    case Op::DIV:
        currentValue_ = (isFloat) ? builder_->CreateFDiv(lhs, rhs, "div")
                                  : builder_->CreateSDiv(lhs, rhs, "div");
        break;
    case Op::MOD:
        currentValue_ = (isFloat) ? builder_->CreateFRem(lhs, rhs, "mod")
                                  : builder_->CreateSRem(lhs, rhs, "mod");
        break;
    case Op::AND:
        currentValue_ = builder_->CreateAnd(lhs, rhs, "and");
        break;
    case Op::OR:
        currentValue_ = builder_->CreateOr(lhs, rhs, "or");
        break;
    case Op::XOR:
        currentValue_ = builder_->CreateXor(lhs, rhs, "xor");
        break;
    case Op::SHL:
        currentValue_ = builder_->CreateShl(lhs, rhs, "shl");
        break;
    case Op::SHR:
        currentValue_ = builder_->CreateAShr(lhs, rhs, "shr");
        break;
    case Op::EQ:
        currentValue_ = builder_->CreateICmpEQ(lhs, rhs, "eq");
        break;
    case Op::NE:
        currentValue_ = builder_->CreateICmpNE(lhs, rhs, "ne");
        break;
    case Op::LT:
        currentValue_ = builder_->CreateICmpSLT(lhs, rhs, "lt");
        break;
    case Op::GT:
        currentValue_ = builder_->CreateICmpSGT(lhs, rhs, "gt");
        break;
    case Op::LE:
        currentValue_ = builder_->CreateICmpSLE(lhs, rhs, "le");
        break;
    case Op::GE:
        currentValue_ = builder_->CreateICmpSGE(lhs, rhs, "ge");
        break;
    case Op::LAND:
        // TODO
        break;
    case Op::LOR:
        // TODO
        break;
    }
}

void CodeGenModule::visit(const Constant &node)
{
    if (valueCategory_ != ValueCategory::RVALUE)
    {
        throw std::runtime_error("Constant to LValue not supported");
    }

    llvm::Type *type = getLLVMType(typeMap_[&node].get());
    currentValue_ = llvm::ConstantInt::get(type, std::stoi(node.value_));
}

void CodeGenModule::visit(const FnCall &node)
{
    if (valueCategory_ != ValueCategory::RVALUE)
    {
        throw std::runtime_error("FnCall to LValue not supported");
    }

    llvm::Function *fn = visitAsFnDesignator(*node.fn_);

    std::vector<llvm::Value *> args;
    if (node.args_)
    {
        for (const auto &arg : node.args_->nodes_)
        {
            std::visit(
                [this, &args](const auto &arg)
                { args.push_back(visitAsRValue(*arg)); },
                arg);
        }
    }

    currentValue_ = builder_->CreateCall(fn, args);
}

void CodeGenModule::visit(const Identifier &node)
{
    if (valueCategory_ == ValueCategory::LVALUE)
    {
        currentValue_ = symbolTable_[node.getID()];
    }
    else if (valueCategory_ == ValueCategory::RVALUE)
    {
        llvm::AllocaInst *alloca = symbolTable_[node.getID()];
        currentValue_ = builder_->CreateLoad(
            alloca->getAllocatedType(), alloca, node.getID());
    }
    else
    {
        currentValue_ = module_->getFunction(node.getID());
    }
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
    if (node.nodes_)
    {
        node.nodes_->accept(*this);
    }
}

void CodeGenModule::visit(const ExprStmt &node)
{
    node.expr_->accept(*this);
}

void CodeGenModule::visit(const For &node)
{
    // Initialize
    std::visit([this](const auto &init) { init->accept(*this); }, node.init_);

    // Create an explict fall through to the condition
    llvm::Function *fn = getCurrentFunction();
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(*context_, "cond", fn);
    builder_->CreateBr(condBB);

    // Condition
    builder_->SetInsertPoint(condBB);
    llvm::Value *cond = visitAsRValue(*node.cond_->expr_);
    cond = builder_->CreateICmpNE(
        cond, llvm::ConstantInt::get(cond->getType(), 0), "loopcond");

    // Loop body
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*context_, "loop", fn);
    llvm::BasicBlock *afterBB =
        llvm::BasicBlock::Create(*context_, "afterloop");
    builder_->CreateCondBr(cond, loopBB, afterBB);
    builder_->SetInsertPoint(loopBB);
    node.body_->accept(*this);
    loopBB = builder_->GetInsertBlock();
    if (node.expr_)
    {
        node.expr_->accept(*this);
    }
    if (!loopBB->getTerminator())
    {
        builder_->CreateBr(condBB);
    }

    // After loop
    fn->insert(fn->end(), afterBB);
    builder_->SetInsertPoint(afterBB);
}

void CodeGenModule::visit(const IfElse &node)
{
    llvm::Value *cond = visitAsRValue(*node.cond_);
    cond = builder_->CreateICmpNE(
        cond, llvm::ConstantInt::get(cond->getType(), 0), "ifcond");

    llvm::Function *fn = getCurrentFunction();

    // Notice only `fn` is passed into thenBB (other blocks are not inserted)
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(*context_, "then", fn);
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(*context_, "ifcont");
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(*context_, "else");
    if (node.elseStmt_)
    {
        builder_->CreateCondBr(cond, thenBB, elseBB);
    }
    else
    {
        builder_->CreateCondBr(cond, thenBB, mergeBB);
    }

    // Then block
    builder_->SetInsertPoint(thenBB);
    node.thenStmt_->accept(*this);
    thenBB = builder_->GetInsertBlock();
    if (!thenBB->getTerminator())
    {
        builder_->CreateBr(mergeBB);
    }

    if (node.elseStmt_)
    {
        // Else block
        fn->insert(fn->end(), elseBB);
        builder_->SetInsertPoint(elseBB);
        node.elseStmt_->accept(*this);
        elseBB = builder_->GetInsertBlock();
        if (!elseBB->getTerminator())
        {
            builder_->CreateBr(mergeBB);
        }
    }

    // Merge block
    if (!thenBB->getTerminator() || !node.elseStmt_ || !elseBB->getTerminator())
    {
        fn->insert(fn->end(), mergeBB);
        builder_->SetInsertPoint(mergeBB);
    }
}

void CodeGenModule::visit(const Return &node)
{
    llvm::Value *retValue = nullptr;
    if (node.expr_)
    {
        retValue = visitAsRValue(*node.expr_);
    }

    if (retValue)
    {
        // Check the type of retValue and cast to expected
        retValue = builder_->CreateCast(
            llvm::Instruction::CastOps::ZExt,
            retValue,
            getLLVMType(node.expr_.get()));

        builder_->CreateRet(retValue);
    }
    else
    {
        builder_->CreateRetVoid();
    }
}

void CodeGenModule::visit(const While &node)
{
    llvm::Function *fn = getCurrentFunction();

    // Create an explict fall through to the condition
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(*context_, "cond", fn);
    builder_->CreateBr(condBB);

    // Condition
    builder_->SetInsertPoint(condBB);
    llvm::Value *cond = visitAsRValue(*node.cond_);
    cond = builder_->CreateICmpNE(
        cond, llvm::ConstantInt::get(cond->getType(), 0), "loopcond");

    // Loop body
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*context_, "loop", fn);
    llvm::BasicBlock *afterBB =
        llvm::BasicBlock::Create(*context_, "afterloop");
    builder_->CreateCondBr(cond, loopBB, afterBB);
    builder_->SetInsertPoint(loopBB);
    node.body_->accept(*this);
    loopBB = builder_->GetInsertBlock();
    if (!loopBB->getTerminator())
    {
        builder_->CreateBr(condBB);
    }

    // After loop
    fn->insert(fn->end(), afterBB);
    builder_->SetInsertPoint(afterBB);
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

llvm::Type *CodeGenModule::getLLVMType(const BaseNode *node)
{
    return getLLVMType(typeMap_[node].get());
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
    else if (auto fnType = dynamic_cast<const FnType *>(type))
    {
        std::vector<llvm::Type *> paramTypes;
        for (const auto &p : fnType->params_->types_)
        {
            paramTypes.push_back(getLLVMType(p.get()));
        }

        return llvm::FunctionType::get(
            getLLVMType(fnType->retType_.get()), paramTypes, false);
    }

    throw std::runtime_error("Unknown type");
}

llvm::Value *CodeGenModule::visitAsLValue(const Expr &expr)
{
    if (expr.isLValue())
    {
        valueCategory_ = ValueCategory::LVALUE;
        expr.accept(*this);
        return currentValue_;
    }

    throw std::runtime_error("Expected LValue");
}

llvm::Value *CodeGenModule::visitAsRValue(const Expr &expr)
{
    // Everything can be decayed into an RValue
    valueCategory_ = ValueCategory::RVALUE;
    expr.accept(*this);
    return currentValue_;
}

llvm::Function *CodeGenModule::visitAsFnDesignator(const Expr &expr)
{
    valueCategory_ = ValueCategory::FN_DESIGNATOR;
    expr.accept(*this);
    return static_cast<llvm::Function *>(currentValue_);
}

} // namespace CodeGen
#include "ty/ty.hpp"
#include <codegen/codegen_ir.hpp>

#include <llvm/IR/Constants.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar/Reassociate.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>

namespace codegen
{
IRCodegen::IRCodegen(std::unique_ptr<ir::IR> ir) : ir(std::move(ir))
{
    context = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    module = std::make_unique<llvm::Module>("Module", *context);
}

void IRCodegen::codegen()
{
    for (const auto &declaration : ir->declarations)
    {
        // codegen(*declaration);
    }

    for (const auto &function : ir->functions)
    {
        codegen(*function);
    }
}

void IRCodegen::print(std::ostream &dst) const
{
    llvm::raw_os_ostream oss(dst);
    module->print(oss, nullptr);
}

void IRCodegen::optimize()
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

    mpm.run(*module.get(), *mam);
}

llvm::Function *IRCodegen::codegen(const ir::FunctionHeader &header)
{
    llvm::Type *ret = to_llvm_type(header.ret.type);
    std::vector<llvm::Type *> args;
    for (const auto &param : header.params)
    {
        args.push_back(to_llvm_type(param.type));
    }

    llvm::FunctionType *ft = llvm::FunctionType::get(ret, args, false);
    llvm::Function *f = llvm::Function::Create(
        ft, llvm::Function::ExternalLinkage, header.name, module.get());

    unsigned i = 0;
    for (auto &arg : f->args())
    {
        if (header.params[i].name != "")
        {
            arg.setName(header.params[i].name);
        }
        i++;
    }

    return f;
}

llvm::Function *IRCodegen::codegen(const ir::Function &function)
{
    llvm::Function *f = codegen(function.header);

    bbs.push_back(llvm::BasicBlock::Create(*context, "entry", f));
    for (int i = 1; i < function.bbs.size(); i++)
    {
        bbs.push_back(
            llvm::BasicBlock::Create(*context, "bb" + std::to_string(i), f));
    }

    builder->SetInsertPoint(bbs[0]);

    for (auto &arg : f->args())
    {
        auto *alloca = builder->CreateAlloca(
            arg.getType(), nullptr, llvm::Twine(arg.getName()));

        named_values[std::string(arg.getName())] = alloca;
        builder->CreateStore(&arg, alloca);
    }

    // function.locals.locals contain the header arguments, filter it out
    for (size_t i = function.header.params.size();
         i < function.locals.locals.size();
         i++)
    {
        auto local = function.locals.locals[i];
        llvm::Type *type = to_llvm_type(local.type);
        std::string name = local.name;
        auto *alloca = builder->CreateAlloca(type, nullptr, name);
        named_values[name] = alloca;
    }
    codegen(*function.bbs[0]);

    for (size_t i = 1; i < function.bbs.size(); i++)
    {
        builder->SetInsertPoint(bbs[i]);
        codegen(*function.bbs[i]);
    }

    llvm::verifyFunction(*f);

    return f;
}

void IRCodegen::codegen(const ir::BasicBlock &block)
{
    for (const auto &stmt : block.statements)
    {
        stmt->accept(*this);
    }
    block.terminator->accept(*this);
}

llvm::Value *IRCodegen::codegen(const ir::BinaryOp &expr)
{
    llvm::Value *lhs = expr.lhs->accept(*this);
    llvm::Value *rhs = expr.rhs->accept(*this);

    if (!lhs || !rhs)
    {
        return nullptr;
    }

    switch (expr.op)
    {
    case ir::BinaryOpType::ADD:
        return builder->CreateAdd(lhs, rhs, "add");
    case ir::BinaryOpType::SUB:
        return builder->CreateSub(lhs, rhs, "sub");
    case ir::BinaryOpType::MUL:
        return builder->CreateMul(lhs, rhs, "mul");
    case ir::BinaryOpType::DIV:
        return builder->CreateSDiv(lhs, rhs, "div");
    case ir::BinaryOpType::MOD:
        return builder->CreateSRem(lhs, rhs, "mod");
    case ir::BinaryOpType::BITWISE_AND:
        return builder->CreateAnd(lhs, rhs, "and");
    case ir::BinaryOpType::BITWISE_OR:
        return builder->CreateOr(lhs, rhs, "or");
    case ir::BinaryOpType::BITWISE_XOR:
        return builder->CreateXor(lhs, rhs, "xor");
    case ir::BinaryOpType::LSL:
        return builder->CreateShl(lhs, rhs, "lsl");
    case ir::BinaryOpType::LSR:
        return builder->CreateLShr(lhs, rhs, "lsr");

    // Comparison operators - returns `i1` type
    case ir::BinaryOpType::EQ:
        return builder->CreateICmpEQ(lhs, rhs, "eq");
    case ir::BinaryOpType::NE:
        return builder->CreateICmpNE(lhs, rhs, "ne");
    case ir::BinaryOpType::LT:
        return builder->CreateICmpSLT(lhs, rhs, "lt");
    case ir::BinaryOpType::GT:
        return builder->CreateICmpSGT(lhs, rhs, "gt");
    case ir::BinaryOpType::LE:
        return builder->CreateICmpSLE(lhs, rhs, "le");
    case ir::BinaryOpType::GE:
        return builder->CreateICmpSGE(lhs, rhs, "ge");
    }
}

llvm::Value *IRCodegen::codegen(const ir::Cast &expr)
{
    return builder->CreateCast(llvm::Instruction::CastOps::ZExt,
        expr.val.accept(*this),
        to_llvm_type(expr.type),
        "cast");
}

llvm::Value *IRCodegen::codegen(const ir::Constant &expr)
{
    switch (expr.type)
    {
    case ty::Types::_BOOL:
        return builder->CreateICmpNE(
            llvm::ConstantInt::get(
                llvm::Type::getInt32Ty(*context), std::stoi(expr.value)),
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 0));
    case ty::Types::INT:
    default:
        return llvm::ConstantInt::getSigned(
            llvm::Type::getInt32Ty(*context), std::stoi(expr.value));
    }
}

llvm::Value *IRCodegen::codegen(const ir::Use &expr)
{
    if (!named_values.count(expr.decl.name))
    {
        return nullptr;
    }

    std::string name = expr.decl.name;
    llvm::AllocaInst *alloca = named_values[name];
    if (!alloca)
    {
        return nullptr;
    }

    return builder->CreateLoad(alloca->getAllocatedType(), alloca, name);
}

llvm::Value *IRCodegen::codegen(const ir::Assign &stmt)
{
    std::string lhs = stmt.lhs.decl.accept(*this);
    llvm::Value *rhs = stmt.rhs->accept(*this);

    // Return case
    if (lhs == "")
    {
        builder->CreateRet(rhs);
    }
    else if (llvm::Value *alloca = named_values[lhs])
    {
        builder->CreateStore(rhs, alloca);
    }
    return nullptr;
}

std::string IRCodegen::codegen(const ir::Declaration &stmt)
{
    return stmt.name;
}

void IRCodegen::codegen(const ir::Return &stmt)
{
}

void IRCodegen::codegen(const ir::Goto &stmt)
{
}

void IRCodegen::codegen(const ir::SwitchInt &stmt)
{
    builder->CreateCondBr(stmt.expr->accept(*this),
        bbs[stmt.vals_to_bb.at(1)],
        bbs[stmt.vals_to_bb.at(0)]);
}

llvm::Type *IRCodegen::to_llvm_type(const ir::Type &type)
{
    return to_llvm_type(type.type);
}

llvm::Type *IRCodegen::to_llvm_type(const ty::Types &type)
{
    switch (type)
    {
    case ty::Types::VOID:
        return llvm::Type::getVoidTy(*context);
    case ty::Types::_BOOL:
        return llvm::Type::getInt1Ty(*context);
    case ty::Types::UNSIGNED_CHAR:
    case ty::Types::CHAR:
        return llvm::Type::getInt8Ty(*context);
    case ty::Types::UNSIGNED_SHORT:
    case ty::Types::SHORT:
        return llvm::Type::getInt16Ty(*context);
    case ty::Types::UNSIGNED_INT:
    case ty::Types::INT:
        return llvm::Type::getInt32Ty(*context);
    case ty::Types::UNSIGNED_LONG:
    case ty::Types::LONG:
        return llvm::Type::getInt64Ty(*context);
    case ty::Types::FLOAT:
        return llvm::Type::getFloatTy(*context);
    case ty::Types::DOUBLE:
        return llvm::Type::getDoubleTy(*context);
    case ty::Types::LONG_DOUBLE:
        return llvm::Type::getFP128Ty(*context);
    }
}

} // namespace codegen

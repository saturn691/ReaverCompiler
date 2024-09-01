#include <codegen/codegen_ir.hpp>

#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar/Reassociate.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>
#include <llvm/Passes/PassBuilder.h>

namespace codegen
{
    IRCodegen::IRCodegen(std::unique_ptr<ir::IR> ir)
        : ir(std::move(ir))
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

        auto mpm = pb.buildPerModuleDefaultPipeline(
            llvm::OptimizationLevel::O2);

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
            ft,
            llvm::Function::ExternalLinkage,
            header.name,
            module.get());

        unsigned i = 0;
        for (auto &arg : f->args())
        {
            if (header.params[i].name)
            {
                arg.setName(header.params[i].name.value());
            }
            i++;
        }

        return f;
    }

    llvm::Function *IRCodegen::codegen(const ir::Function &function)
    {
        llvm::Function *f = codegen(function.header);
        llvm::BasicBlock *bb = llvm::BasicBlock::Create(
            *context, "entry", f);
        builder->SetInsertPoint(bb);

        for (auto &arg : f->args())
        {
            auto *alloca = builder->CreateAlloca(
                arg.getType(), nullptr, llvm::Twine(arg.getName()));

            named_values[std::string(arg.getName())] = alloca;
            builder->CreateStore(&arg, alloca);
        }
        for (const auto &local : function.locals.locals)
        {
            llvm::Type *type = to_llvm_type(local.type);
            std::string name = local.name.value();
            auto *alloca = builder->CreateAlloca(type, nullptr, name);
            named_values[name] = alloca;
        }
        codegen(*function.bbs[0]);

        for (size_t i = 1; i < function.bbs.size(); i++)
        {
            llvm::BasicBlock *bb = llvm::BasicBlock::Create(
                *context, "bb" + std::to_string(i), f);
            builder->SetInsertPoint(bb);
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
        case ir::BinaryOpType::LOGICAL_AND:
            return builder->CreateAnd(lhs, rhs, "and");
        case ir::BinaryOpType::LOGICAL_OR:
            return builder->CreateOr(lhs, rhs, "or");
        }
    }

    llvm::Value *IRCodegen::codegen(const ir::Constant &expr)
    {
        return nullptr;
    }

    llvm::Value *IRCodegen::codegen(const ir::Use &expr)
    {
        if (!named_values.count(expr.decl.name.value()))
        {
            return nullptr;
        }

        std::string name = expr.decl.name.value();
        llvm::AllocaInst *alloca = named_values[name];
        if (!alloca)
        {
            return nullptr;
        }

        return builder->CreateLoad(alloca->getAllocatedType(), alloca, name);
    }

    llvm::Value *IRCodegen::codegen(const ir::Assign &stmt)
    {
        std::optional<std::string> lhs = stmt.lhs.accept(*this);
        llvm::Value *rhs = stmt.rhs->accept(*this);

        // Return case
        if (!lhs.has_value())
        {
            builder->CreateRet(rhs);
        }
        else
        {
            llvm::Value *alloca = named_values[lhs.value()];
            builder->CreateStore(rhs, alloca);
        }
        return nullptr;
    }

    std::optional<std::string> IRCodegen::codegen(const ir::Declaration &stmt)
    {
        return stmt.name;
    }

    llvm::Type *IRCodegen::to_llvm_type(const ir::Type &type)
    {
        return to_llvm_type(type.type);
    }

    llvm::Type *IRCodegen::to_llvm_type(const ir::Types &type)
    {
        switch (type)
        {
        case ir::Types::VOID:
            return llvm::Type::getVoidTy(*context);
        case ir::Types::UNSIGNED_CHAR:
        case ir::Types::CHAR:
            return llvm::Type::getInt8Ty(*context);
        case ir::Types::UNSIGNED_SHORT:
        case ir::Types::SHORT:
            return llvm::Type::getInt16Ty(*context);
        case ir::Types::UNSIGNED_INT:
        case ir::Types::INT:
            return llvm::Type::getInt32Ty(*context);
        case ir::Types::UNSIGNED_LONG:
        case ir::Types::LONG:
            return llvm::Type::getInt64Ty(*context);
        case ir::Types::FLOAT:
            return llvm::Type::getFloatTy(*context);
        case ir::Types::DOUBLE:
            return llvm::Type::getDoubleTy(*context);
        case ir::Types::LONG_DOUBLE:
            return llvm::Type::getFP128Ty(*context);
        }
    }

}

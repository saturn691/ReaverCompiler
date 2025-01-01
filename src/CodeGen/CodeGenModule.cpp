#include "CodeGen/CodeGenModule.hpp"

#include "AST/Decl.hpp"
#include "AST/Expr.hpp"
#include "AST/Stmt.hpp"
#include "AST/Type.hpp"

#include "CodeGen/ScopeGuard.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
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

CodeGenModule::CodeGenModule(std::string outputFile, TypeMap &typeMap)
    : outputFile_(std::move(outputFile)), typeMap_(typeMap),
      context_(std::make_unique<llvm::LLVMContext>()),
      builder_(std::make_unique<llvm::IRBuilder<>>(*context_)),
      module_(std::make_unique<llvm::Module>("Module", *context_))
{
    // Initialise all the targets for emitting object code
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    llvm::InitializeAllAsmParsers();
}

void CodeGenModule::emitLLVM()
{
    std::error_code ec;
    llvm::raw_fd_ostream os(outputFile_, ec);

    if (ec)
    {
        llvm::errs() << "Could not open file: " << ec.message() << "\n";
        return;
    }

    module_->print(os, nullptr);
}

void CodeGenModule::emitObject()
{
    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    auto CPU = "generic";
    auto features = "";

    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    if (!target)
    {
        llvm::errs() << error;
    }

    // PIC = Position Independent Code
    llvm::TargetOptions opt;
    auto targetMachine = target->createTargetMachine(
        targetTriple, CPU, features, opt, llvm::Reloc::Model::PIC_);

    module_->setDataLayout(targetMachine->createDataLayout());
    module_->setTargetTriple(targetTriple);

    // Emit the code
    llvm::legacy::PassManager passManager;
    auto fileType = llvm::CodeGenFileType::ObjectFile;
    std::error_code ec;
    llvm::raw_fd_ostream os(outputFile_, ec);

    if (ec)
    {
        llvm::errs() << "Could not open file: " << ec.message() << "\n";
        return;
    }

    if (targetMachine->addPassesToEmitFile(passManager, os, nullptr, fileType))
    {
        llvm::errs() << "TargetMachine can't emit a file of this type\n";
        return;
    }

    passManager.run(*module_);
    os.flush();
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

void CodeGenModule::visit(const ArrayDecl &node)
{
    // Allocate memory for the array
    node.decl_->accept(*this);
}

void CodeGenModule::visit(const BasicTypeDecl &node)
{
    // Intentionally left blank
}

void CodeGenModule::visit(const DeclNode &node)
{
    // For struct/union definitions
    node.type_->accept(*this);

    // Allocate memory for the variable
    if (node.initDeclList_)
    {
        node.initDeclList_->accept(*this);
    }
}

void CodeGenModule::visit(const DefinedTypeDecl &node)
{
    // Intentionally left blank
}

void CodeGenModule::visit(const Enum &node)
{
    // Register the constants
    if (auto *enumType = dynamic_cast<const EnumType *>(typeMap_[&node].get()))
    {
        for (const auto &member : enumType->consts_)
        {
            constTablePush(
                member.first,
                llvm::ConstantInt::get(
                    llvm::Type::getInt32Ty(*context_), member.second, false));
        }
    }
}

void CodeGenModule::visit(const EnumMember &node)
{
    // Intentionally left blank
}

void CodeGenModule::visit(const EnumMemberList &node)
{
    // Intentionally left blank
}

void CodeGenModule::visit(const FnDecl &node)
{
    // Label the function arguments
    node.params_->accept(*this);
}

void CodeGenModule::visit(const FnDef &node)
{
    std::string fnName = node.decl_->getID();
    llvm::Function *fn = module_->getFunction(fnName);

    // Safe to do... this was checked in the TypeChecker
    const FnType *type = dynamic_cast<const FnType *>(typeMap_[&node].get());
    llvm::Type *retType = getLLVMType(type->retType_.get());

    // If we haven't declared the function yet, create it
    if (!fn)
    {
        std::vector<llvm::Type *> paramTypes;
        for (const auto &p : type->params_->types_)
        {
            paramTypes.push_back(getLLVMType(p.second.get()));
        }

        llvm::FunctionType *fnType =
            llvm::FunctionType::get(retType, paramTypes, false);

        fn = llvm::Function::Create(
            fnType, llvm::Function::ExternalLinkage, fnName, module_.get());
    }

    llvm::BasicBlock *bb = llvm::BasicBlock::Create(*context_, "entry", fn);
    builder_->SetInsertPoint(bb);

    // Required, quick solution to scopes being inside compound statements
    // Works the same, but we have 1 extra scope that only contains the function
    // arguments
    pushScope();

    // Label the function arguments
    node.decl_->accept(*this);
    for (auto &arg : fn->args())
    {
        std::string paramName = arg.getName().str();
        llvm::AllocaInst *allocaInst =
            builder_->CreateAlloca(arg.getType(), nullptr, arg.getName());
        symbolTablePush(paramName, allocaInst);
        builder_->CreateStore(&arg, allocaInst);
    }

    // Generate the function body
    isGlobal_ = false;
    node.body_->accept(*this);
    isGlobal_ = true;

    // If the function does not have a return statement, add one
    if (!builder_->GetInsertBlock()->getTerminator())
    {
        if (retType->isVoidTy())
        {
            builder_->CreateRetVoid();
        }
        else
        {
            builder_->CreateRet(llvm::ConstantInt::get(retType, 0, false));
        }
    }

    popScope();

    // Attributes required for strings
    fn->addFnAttr(llvm::Attribute::NoInline);
    fn->addFnAttr(llvm::Attribute::NoUnwind);
    fn->addFnAttr(llvm::Attribute::OptimizeNone);
}

void CodeGenModule::visit(const InitDecl &node)
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
    else if (isGlobal_)
    {
        if (type->isArrayTy())
        {
            // Global array declaration
            module_->getOrInsertGlobal(node.getID(), type);
        }
    }
    else
    {
        // Allocate memory for the variable
        llvm::AllocaInst *alloca =
            builder_->CreateAlloca(type, nullptr, node.getID());
        symbolTablePush(node.getID(), alloca);

        if (node.expr_)
        {
            llvm::Value *initVal = visitAsRValue(*node.expr_);
            builder_->CreateStore(initVal, alloca);
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

void CodeGenModule::visit(const PtrDecl &node)
{
    node.decl_->accept(*this);
}

void CodeGenModule::visit(const PtrNode &node)
{
    // Intentionally left blank
}

void CodeGenModule::visit(const Struct &node)
{
    // Treated the exact same way as FnDef
    if (node.name_.empty())
    {
        // Anonymous struct, only useful inside structs
        return;
    }
    if (!node.members_)
    {
        // Forward declaration or used as a type
        return;
    }

    // All information is available in the typeMap
    const StructType *type =
        dynamic_cast<const StructType *>(typeMap_[&node].get());
    std::vector<llvm::Type *> memberTypes;
    for (const auto &member : type->params_->types_)
    {
        memberTypes.push_back(getLLVMType(member.second.get()));
    }

    // Set the type in LLVM
    llvm::StructType *structType =
        llvm::StructType::create(*context_, "struct." + node.name_);
    structType->setBody(memberTypes);
}

void CodeGenModule::visit(const StructDecl &node)
{
    // Intentionally left blank
}

void CodeGenModule::visit(const StructDeclList &node)
{
    // Intentionally left blank
}

void CodeGenModule::visit(const StructMember &node)
{
    // Intentionally left blank
}

void CodeGenModule::visit(const StructMemberList &node)
{
    // Intentionally left blank
}

void CodeGenModule::visit(const TranslationUnit &node)
{
    // For global declarations
    pushScope();

    for (const auto &decl : node.nodes_)
    {
        std::visit([this](const auto &decl) { decl->accept(*this); }, decl);
    }

    popScope();
}

void CodeGenModule::visit(const Typedef &node)
{
    // Intentionally left blank
}

/******************************************************************************
 *                          Expressions                                       *
 *                  Must separate LValue and RValue handling                  *
 *****************************************************************************/

void CodeGenModule::visit(const ArrayAccess &node)
{
    auto valueCategory = valueCategory_;
    llvm::Value *index = visitAsRValue(*node.index_);
    llvm::Type *elementType = getLLVMType(&node);
    llvm::Value *arrayPtr;

    if (getLLVMType(node.arr_.get())->isPointerTy())
    {
        // Load the pointer first
        llvm::Value *arr = visitAsRValue(*node.arr_);
        arrayPtr = builder_->CreateInBoundsGEP(elementType, arr, index, "gep");
    }
    else
    {
        llvm::Value *arr = visitAsLValue(*node.arr_);
        llvm::Value *zero =
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
        arrayPtr = builder_->CreateInBoundsGEP(
            getLLVMType(node.arr_.get()), arr, {zero, index}, "gep");
    }

    if (valueCategory == ValueCategory::LVALUE)
    {
        currentValue_ = arrayPtr;
    }
    else
    {
        currentValue_ = builder_->CreateLoad(elementType, arrayPtr, "load");
    }
}

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
        builder_->CreateStore(rhs, lhs);
        currentValue_ = rhs;
        break;
    case Op::ADD_ASSIGN:
        llvm::Value *lhsVal = visitAsRValue(*node.lhs_);
        currentValue_ =
            builder_->CreateStore(builder_->CreateAdd(lhsVal, rhs), lhs);
        break;
    }
}

void CodeGenModule::visit(const ArgExprList &node)
{
    // Intentionally left blank
}

void CodeGenModule::visit(const BinaryOp &node)
{
    using Op = BinaryOp::Op;

    if (valueCategory_ == ValueCategory::LVALUE)
    {
        throw std::runtime_error("BinaryOp to LValue not supported");
    }

    if (node.op_ == Op::LAND || node.op_ == Op::LOR)
    {
        visitLogicalOp(node);
        return;
    }

    llvm::Value *lhs = visitAsRValue(*node.lhs_);
    llvm::Value *rhs = visitAsRValue(*node.rhs_);

    lhs = runUsualArithmeticConversions(
        typeMap_[node.lhs_.get()].get(), typeMap_[node.rhs_.get()].get(), lhs);
    rhs = runUsualArithmeticConversions(
        typeMap_[node.rhs_.get()].get(), typeMap_[node.lhs_.get()].get(), rhs);

    bool isFloat = lhs->getType()->isFloatingPointTy();

    switch (node.op_)
    {
    case Op::ADD:
        if (lhs->getType()->isPointerTy() || rhs->getType()->isPointerTy())
        {
            // Pointer arithmetic (getelementptr inbounds)
            if (rhs->getType()->isPointerTy())
            {
                // Pointer becomes lhs
                std::swap(lhs, rhs);
            }

            currentValue_ = builder_->CreateInBoundsGEP(
                getPointerElementType(&node), lhs, rhs, "add");
        }
        else
        {
            currentValue_ = (isFloat) ? builder_->CreateFAdd(lhs, rhs, "add")
                                      : builder_->CreateAdd(lhs, rhs, "add");
        }
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
    case Op::LOR:
        // Handled above
        break;
    }
}

void CodeGenModule::visitLogicalOp(const BinaryOp &node)
{
    using Op = BinaryOp::Op;

    llvm::Function *fn = getCurrentFunction();
    llvm::BasicBlock *lhsBB = builder_->GetInsertBlock();
    llvm::BasicBlock *rhsBB = llvm::BasicBlock::Create(*context_, "rhs");
    llvm::BasicBlock *afterBB = llvm::BasicBlock::Create(*context_, "after");
    auto *lhsType = typeMap_[node.lhs_.get()].get();
    auto *rhsType = typeMap_[node.rhs_.get()].get();

    // LHS
    llvm::Value *lhs = visitAsRValue(*node.lhs_);
    lhs = runUsualArithmeticConversions(lhsType, rhsType, lhs);
    llvm::Value *zero = llvm::ConstantInt::get(lhs->getType(), 0);
    llvm::Value *lhsBool = builder_->CreateICmpNE(lhs, zero, "lhsVal");
    lhsBB = builder_->GetInsertBlock();

    if (node.op_ == Op::LAND)
    {
        // True - check other condition is true
        builder_->CreateCondBr(lhsBool, rhsBB, afterBB);
    }
    else
    {
        builder_->CreateCondBr(lhsBool, afterBB, rhsBB);
    }

    // RHS
    fn->insert(fn->end(), rhsBB);
    builder_->SetInsertPoint(rhsBB);
    llvm::Value *rhs = visitAsRValue(*node.rhs_);
    rhs = runUsualArithmeticConversions(rhsType, lhsType, rhs);
    llvm::Value *rhsBool = builder_->CreateICmpNE(rhs, zero, "rhsVal");
    builder_->CreateBr(afterBB);
    rhsBB = builder_->GetInsertBlock();

    // After
    fn->insert(fn->end(), afterBB);
    builder_->SetInsertPoint(afterBB);
    llvm::PHINode *phi =
        builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "phi");
    phi->addIncoming(lhsBool, lhsBB);
    phi->addIncoming(rhsBool, rhsBB);

    currentValue_ = phi;
}

void CodeGenModule::visit(const Constant &node)
{
    if (valueCategory_ != ValueCategory::RVALUE)
    {
        throw std::runtime_error("Constant to LValue not supported");
    }

    llvm::Type *type = getLLVMType(typeMap_[&node].get());
    if (type->isIntegerTy(8))
    {
        // Gets the 1 or 2 characters between the single quotes
        currentValue_ = llvm::ConstantInt::get(type, node.getChar());
    }
    else if (type->isFloatTy())
    {
        currentValue_ = llvm::ConstantFP::get(type, std::stof(node.value_));
    }
    else
    {
        currentValue_ = llvm::ConstantInt::get(type, std::stoi(node.value_));
    }
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
        currentValue_ = symbolTableLookup(node.getID());
    }
    else if (valueCategory_ == ValueCategory::RVALUE)
    {
        if (llvm::AllocaInst *alloca = symbolTableLookup(node.getID()))
        {
            currentValue_ = builder_->CreateLoad(
                alloca->getAllocatedType(), alloca, node.getID());
        }
        else if (llvm::Constant *constant = constTableLookup(node.getID()))
        {
            currentValue_ = constant;
        }
        else
        {
            throw std::runtime_error("Unknown identifier: " + node.getID());
        }
    }
    else
    {
        currentValue_ = module_->getFunction(node.getID());
    }
}

void CodeGenModule::visit(const Paren &node)
{
    // Intentionally don't use the LValue/RValue handling
    // This method is "blind" to the value category, it passes it down
    node.expr_->accept(*this);
}

void CodeGenModule::visit(const SizeOf &node)
{
    if (valueCategory_ != ValueCategory::RVALUE)
    {
        throw std::runtime_error("SizeOf to LValue not supported");
    }

    llvm::Type *type = (node.expr_) ? getLLVMType(node.expr_.get())
                                    : getLLVMType(node.type_->getType().get());

    currentValue_ = llvm::ConstantInt::get(
        llvm::Type::getInt64Ty(*context_),
        module_->getDataLayout().getTypeAllocSize(type));
}

void CodeGenModule::visit(const StringLiteral &node)
{
    if (valueCategory_ != ValueCategory::RVALUE)
    {
        throw std::runtime_error("StringLiteral to LValue not supported");
    }

    llvm::GlobalVariable *str = builder_->CreateGlobalString(node.value_);
    llvm::Value *zero =
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
    llvm::Value *indices[] = {zero, zero};
    currentValue_ =
        builder_->CreateInBoundsGEP(str->getValueType(), str, indices, "str");
}

void CodeGenModule::visit(const StructAccess &node)
{
    auto valueCategory = valueCategory_;

    llvm::Value *structPtr = visitAsLValue(*node.expr_);
    auto structType =
        dynamic_cast<const StructType *>(typeMap_[node.expr_.get()].get());
    auto index = structType->getMemberIndex(node.member_);
    llvm::Value *indices[] = {
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0),
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), index)};

    llvm::Value *memberPtr = builder_->CreateInBoundsGEP(
        getLLVMType(node.expr_.get()), structPtr, indices, "gep");

    if (valueCategory == ValueCategory::LVALUE)
    {
        currentValue_ = memberPtr;
    }
    else
    {
        currentValue_ =
            builder_->CreateLoad(getLLVMType(&node), memberPtr, "load");
    }
}

void CodeGenModule::visit(const StructPtrAccess &node)
{
}

void CodeGenModule::visit(const TernaryOp &node)
{
    if (valueCategory_ == ValueCategory::LVALUE)
    {
        throw std::runtime_error("TernaryOp to LValue not supported");
    }

    llvm::Function *fn = getCurrentFunction();
    llvm::BasicBlock *lhsBB = llvm::BasicBlock::Create(*context_, "lhs", fn);
    llvm::BasicBlock *rhsBB = llvm::BasicBlock::Create(*context_, "rhs");
    llvm::BasicBlock *afterBB = llvm::BasicBlock::Create(*context_, "after");
    auto lhsType = typeMap_[node.lhs_.get()].get();
    auto rhsType = typeMap_[node.rhs_.get()].get();
    llvm ::Type *type = getLLVMType(&node);

    llvm::Value *cond = visitAsRValue(*node.cond_);
    llvm::Value *zero = llvm::ConstantInt::get(cond->getType(), 0);
    llvm::Value *condBool = builder_->CreateICmpNE(cond, zero, "cond");

    // True - evaluate LHS. False - evaluate RHS.
    builder_->CreateCondBr(condBool, lhsBB, rhsBB);

    // LHS
    builder_->SetInsertPoint(lhsBB);
    llvm::Value *lhs = visitAsRValue(*node.lhs_);
    lhs = runUsualArithmeticConversions(lhsType, rhsType, lhs);
    builder_->CreateBr(afterBB);
    lhsBB = builder_->GetInsertBlock();

    // RHS
    fn->insert(fn->end(), rhsBB);
    builder_->SetInsertPoint(rhsBB);
    llvm::Value *rhs = visitAsRValue(*node.rhs_);
    rhs = runUsualArithmeticConversions(rhsType, lhsType, rhs);
    builder_->CreateBr(afterBB);
    rhsBB = builder_->GetInsertBlock();

    // After
    fn->insert(fn->end(), afterBB);
    builder_->SetInsertPoint(afterBB);
    llvm::PHINode *phi = builder_->CreatePHI(type, 2, "phi");
    phi->addIncoming(lhs, lhsBB);
    phi->addIncoming(rhs, rhsBB);

    currentValue_ = phi;
}

void CodeGenModule::visit(const UnaryOp &node)
{
    // The only LValue is DEREF
    if (valueCategory_ == ValueCategory::LVALUE)
    {
        if (node.op_ != UnaryOp::Op::DEREF)
        {
            throw std::runtime_error("UnaryOp to LValue not supported");
        }
        // Looks weird, but we need to load the pointer, not get the address
        currentValue_ = visitAsRValue(*node.expr_);
    }
    else
    {
        llvm::Value *expr;

        switch (node.op_)
        {
        case UnaryOp::Op::ADDR:
            expr = visitAsLValue(*node.expr_);
            currentValue_ = expr;
            break;
        case UnaryOp::Op::DEREF:
            expr = visitAsRValue(*node.expr_);
            // Needs to find out the type again, because of opaque pointers
            currentValue_ =
                builder_->CreateLoad(getLLVMType(&node), expr, "deref");
            break;
        case UnaryOp::Op::PLUS:
            expr = visitAsRValue(*node.expr_);
            currentValue_ = expr;
            break;
        case UnaryOp::Op::MINUS:
            expr = visitAsRValue(*node.expr_);
            currentValue_ = builder_->CreateNeg(expr, "neg");
            break;
        case UnaryOp::Op::NOT:
            expr = visitAsRValue(*node.expr_);
            currentValue_ = builder_->CreateNot(expr, "not");
            break;
        case UnaryOp::Op::LNOT:
            expr = visitAsRValue(*node.expr_);
            currentValue_ = builder_->CreateICmpEQ(
                expr, llvm::ConstantInt::get(expr->getType(), 0), "lnot");
            break;
        case UnaryOp::Op::POST_DEC:
            // `x--` is equivalent to `x = x - 1; return x;`
            expr = visitAsRValue(*node.expr_);
            builder_->CreateStore(
                builder_->CreateSub(
                    expr,
                    llvm::ConstantInt::get(expr->getType(), 1),
                    "postdec"),
                visitAsLValue(*node.expr_));
            break;
        case UnaryOp::Op::POST_INC:
            // `x++` is equivalent to `x = x + 1; return x;`
            expr = visitAsRValue(*node.expr_);
            builder_->CreateStore(
                builder_->CreateAdd(
                    expr,
                    llvm::ConstantInt::get(expr->getType(), 1),
                    "postinc"),
                visitAsLValue(*node.expr_));
            break;
        case UnaryOp::Op::PRE_DEC:
            expr = visitAsRValue(*node.expr_);
            currentValue_ = builder_->CreateSub(
                expr, llvm::ConstantInt::get(expr->getType(), 1), "postdec");
            break;
        case UnaryOp::Op::PRE_INC:
            expr = visitAsRValue(*node.expr_);
            currentValue_ = builder_->CreateAdd(
                expr, llvm::ConstantInt::get(expr->getType(), 1), "postinc");
            break;
        }
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

void CodeGenModule::visit(const Break &node)
{
    llvm::BasicBlock *toBB = breakStack_.top();
    builder_->CreateBr(toBB);
}

void CodeGenModule::visit(const Case &node)
{
    llvm::BasicBlock *caseBB =
        llvm::BasicBlock::Create(*context_, "case", getCurrentFunction());

    // If current case doesn't break, it will fall through to the next case
    if (!builder_->GetInsertBlock()->getTerminator())
    {
        builder_->CreateBr(caseBB);
    }

    builder_->SetInsertPoint(caseBB);
    node.body_->accept(*this);

    // Add the case to the switch
    if (node.expr_)
    {
        // Must evaluate, it is a constant expression
        // Must be an int (or unsigned int) due to integer promotion
        int value = node.expr_->eval().value();
        currentSwitch_->addCase(
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), value),
            caseBB);
    }
    else
    {
        // Default case
        currentSwitch_->setDefaultDest(caseBB);
    }
}

void CodeGenModule::visit(const CompoundStmt &node)
{
    if (node.nodes_)
    {
        pushScope();
        node.nodes_->accept(*this);
        popScope();
    }
}

void CodeGenModule::visit(const Continue &node)
{
    llvm::BasicBlock *toBB = continueStack_.top();
    builder_->CreateBr(toBB);
}

void CodeGenModule::visit(const DoWhile &node)
{
    llvm::Function *fn = getCurrentFunction();
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*context_, "loop");
    llvm::BasicBlock *afterBB =
        llvm::BasicBlock::Create(*context_, "afterloop");
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(*context_, "cond");

    // Loop body
    ScopeGuard<std::stack<llvm::BasicBlock *>> sg(breakStack_, afterBB);
    ScopeGuard<std::stack<llvm::BasicBlock *>> sg2(continueStack_, condBB);
    fn->insert(fn->end(), loopBB);
    builder_->CreateBr(loopBB);
    builder_->SetInsertPoint(loopBB);
    node.body_->accept(*this);
    if (!builder_->GetInsertBlock()->getTerminator())
    {
        builder_->CreateBr(condBB);
    }

    // Loop expression
    fn->insert(fn->end(), condBB);
    builder_->SetInsertPoint(condBB);
    llvm::Value *cond = visitAsRValue(*node.cond_);
    cond = builder_->CreateICmpNE(
        cond, llvm::ConstantInt::get(cond->getType(), 0), "loopcond");
    builder_->CreateCondBr(cond, loopBB, afterBB);

    // After loop
    fn->insert(fn->end(), afterBB);
    builder_->SetInsertPoint(afterBB);
}

void CodeGenModule::visit(const ExprStmt &node)
{
    if (node.expr_)
    {
        visitAsRValue(*node.expr_);
    }
}

void CodeGenModule::visit(const For &node)
{
    llvm::Function *fn = getCurrentFunction();
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*context_, "loop");
    llvm::BasicBlock *loopExprBB =
        llvm::BasicBlock::Create(*context_, "loopexpr");
    llvm::BasicBlock *afterBB =
        llvm::BasicBlock::Create(*context_, "afterloop");

    // Initialize
    pushScope(); // `int i = 0;` has a different scope
    std::visit([this](const auto &init) { init->accept(*this); }, node.init_);

    // Create an explict fall through to the condition
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(*context_, "cond", fn);
    builder_->CreateBr(condBB);

    // Condition
    builder_->SetInsertPoint(condBB);
    if (node.cond_->expr_)
    {
        llvm::Value *cond = visitAsRValue(*node.cond_->expr_);
        cond = builder_->CreateICmpNE(
            cond, llvm::ConstantInt::get(cond->getType(), 0), "loopcond");
        builder_->CreateCondBr(cond, loopBB, afterBB);
    }
    else
    {
        builder_->CreateBr(loopBB);
    }

    // Loop body
    ScopeGuard<std::stack<llvm::BasicBlock *>> sg(breakStack_, afterBB);
    ScopeGuard<std::stack<llvm::BasicBlock *>> sg2(continueStack_, loopExprBB);
    fn->insert(fn->end(), loopBB);
    builder_->SetInsertPoint(loopBB);
    node.body_->accept(*this);
    loopBB = builder_->GetInsertBlock();
    if (!loopBB->getTerminator())
    {
        builder_->CreateBr(loopExprBB);
    }

    // Loop expression
    fn->insert(fn->end(), loopExprBB);
    builder_->SetInsertPoint(loopExprBB);
    if (node.expr_)
    {
        visitAsRValue(*node.expr_);
    }
    builder_->CreateBr(condBB);

    // After loop
    fn->insert(fn->end(), afterBB);
    builder_->SetInsertPoint(afterBB);
    popScope();
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
    bool insertMergeBB = !node.elseStmt_;

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
        insertMergeBB = true;
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
            insertMergeBB = true;
        }
    }

    // Merge block
    if (insertMergeBB)
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
        llvm::Type *expectedType = getCurrentFunction()->getReturnType();
        retValue = builder_->CreateCast(
            llvm::Instruction::CastOps::ZExt, retValue, expectedType);

        builder_->CreateRet(retValue);
    }
    else
    {
        builder_->CreateRetVoid();
    }
}

void CodeGenModule::visit(const Switch &node)
{
    llvm::Function *fn = getCurrentFunction();
    llvm::BasicBlock *mergeBB =
        llvm::BasicBlock::Create(*context_, "switchcont");

    llvm::Value *switchValue = visitAsRValue(*node.expr_);

    // mergeBB is default destination, may be replaced
    llvm::SwitchInst *switchInst = builder_->CreateSwitch(switchValue, mergeBB);
    ScopeGuard<llvm::SwitchInst *> sg(currentSwitch_, switchInst);
    ScopeGuard<std::stack<llvm::BasicBlock *>> sg2(breakStack_, mergeBB);

    node.body_->accept(*this);

    // Fall through to the end of the switch
    if (!builder_->GetInsertBlock()->getTerminator())
    {
        builder_->CreateBr(mergeBB);
    }

    // Merge block
    fn->insert(fn->end(), mergeBB);
    builder_->SetInsertPoint(mergeBB);
}

void CodeGenModule::visit(const While &node)
{
    llvm::Function *fn = getCurrentFunction();
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*context_, "loop");
    llvm::BasicBlock *afterBB =
        llvm::BasicBlock::Create(*context_, "afterloop");

    // Create an explict fall through to the condition
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(*context_, "cond", fn);
    builder_->CreateBr(condBB);

    // Condition
    builder_->SetInsertPoint(condBB);
    llvm::Value *cond = visitAsRValue(*node.cond_);
    cond = builder_->CreateICmpNE(
        cond, llvm::ConstantInt::get(cond->getType(), 0), "loopcond");

    // Loop body
    ScopeGuard<std::stack<llvm::BasicBlock *>> sg(breakStack_, afterBB);
    ScopeGuard<std::stack<llvm::BasicBlock *>> sg2(continueStack_, condBB);
    fn->insert(fn->end(), loopBB);
    builder_->CreateCondBr(cond, loopBB, afterBB);
    builder_->SetInsertPoint(loopBB);
    node.body_->accept(*this);
    if (!builder_->GetInsertBlock()->getTerminator())
    {
        builder_->CreateBr(condBB);
    }

    // After loop
    fn->insert(fn->end(), afterBB);
    builder_->SetInsertPoint(afterBB);
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
        return getLLVMType(basicType->type_);
    }
    else if (auto fnType = dynamic_cast<const FnType *>(type))
    {
        std::vector<llvm::Type *> paramTypes;
        for (const auto &p : fnType->params_->types_)
        {
            paramTypes.push_back(getLLVMType(p.second.get()));
        }

        return llvm::FunctionType::get(
            getLLVMType(fnType->retType_.get()), paramTypes, false);
    }
    else if (auto ptrType = dynamic_cast<const PtrType *>(type))
    {
        // Opaque pointers do not contain the type they point to
        return llvm::PointerType::get(*context_, 0);
    }
    else if (auto arrType = dynamic_cast<const ArrayType *>(type))
    {
        return llvm::ArrayType::get(
            getLLVMType(arrType->type_.get()), arrType->size_);
    }
    else if (auto structType = dynamic_cast<const StructType *>(type))
    {
        // Structs are not defined here
        std::string name = "struct." + structType->name_;
        return llvm::StructType::getTypeByName(*context_, name);
    }
    else if (auto enumType = dynamic_cast<const EnumType *>(type))
    {
        // Enums are not defined here
        return llvm::Type::getInt32Ty(*context_);
    }

    throw std::runtime_error("Unknown type");
}

llvm::Type *CodeGenModule::getLLVMType(Types ty)
{
    switch (ty)
    {
    case Types::VOID:
        return llvm::Type::getVoidTy(*context_);
    case Types::BOOL:
        return llvm::Type::getInt1Ty(*context_);
    case Types::CHAR:
    case Types::UNSIGNED_CHAR:
        return llvm::Type::getInt8Ty(*context_);
    case Types::SHORT:
    case Types::UNSIGNED_SHORT:
        return llvm::Type::getInt16Ty(*context_);
    case Types::INT:
    case Types::UNSIGNED_INT:
        return llvm::Type::getInt32Ty(*context_);
    case Types::LONG:
    case Types::UNSIGNED_LONG:
        return llvm::Type::getInt64Ty(*context_);
    case Types::FLOAT:
        return llvm::Type::getFloatTy(*context_);
    case Types::DOUBLE:
        return llvm::Type::getDoubleTy(*context_);
    case Types::LONG_DOUBLE:
        return llvm::Type::getFP128Ty(*context_);
    }

    throw std::runtime_error("Unknown type");
}

llvm::Type *CodeGenModule::getPointerElementType(const BaseNode *node)
{
    if (auto *ty = dynamic_cast<const PtrType *>(typeMap_[node].get()))
    {
        return getLLVMType(ty->type_.get());
    }
    else if (auto *ty = dynamic_cast<const ArrayType *>(typeMap_[node].get()))
    {
        return getLLVMType(ty->type_.get());
    }

    throw std::runtime_error("Expected PtrType");
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

void CodeGenModule::symbolTablePush(std::string id, llvm::AllocaInst *alloca)
{
    symbolTable_.back()[id] = alloca;
}

llvm::AllocaInst *CodeGenModule::symbolTableLookup(std::string id) const
{
    for (auto it = symbolTable_.rbegin(); it != symbolTable_.rend(); ++it)
    {
        auto search = it->find(id);
        if (search != it->end())
        {
            return search->second;
        }
    }

    return nullptr;
}

void CodeGenModule::constTablePush(std::string id, llvm::Constant *constant)
{
    constTable_.back()[id] = constant;
}

llvm::Constant *CodeGenModule::constTableLookup(std::string id) const
{
    for (auto it = constTable_.rbegin(); it != constTable_.rend(); ++it)
    {
        auto search = it->find(id);
        if (search != it->end())
        {
            return search->second;
        }
    }

    return nullptr;
}

void CodeGenModule::pushScope()
{
    symbolTable_.push_back(
        std::unordered_map<std::string, llvm::AllocaInst *>());
    constTable_.push_back(std::unordered_map<std::string, llvm::Constant *>());
}

void CodeGenModule::popScope()
{
    symbolTable_.pop_back();
    constTable_.pop_back();
}

llvm::Value *CodeGenModule::runUsualArithmeticConversions(
    const BaseType *lhs,
    const BaseType *rhs,
    llvm::Value *val)
{
    auto lhsBasic = dynamic_cast<const BasicType *>(lhs);
    auto rhsBasic = dynamic_cast<const BasicType *>(rhs);

    if (!lhsBasic || !rhsBasic)
    {
        // Could be pointer type, in which case no conversion is needed
        return val;
    }

    Types t = TypeChecker::runUsualArithmeticConversions(
        lhsBasic->type_, rhsBasic->type_);

    // sext is not allowed for i1 (because 1 -> -1)
    // doesn't matter if we want to promote to signed int
    if (BasicType(t).isSigned() && !val->getType()->isIntegerTy(1))
    {
        return builder_->CreateSExt(val, getLLVMType(t));
    }

    return builder_->CreateZExt(val, getLLVMType(t));
}

void CodeGenModule::runIntegerPromotions(
    const BaseType *type,
    llvm::Value *&val)
{
    auto basicType = dynamic_cast<const BasicType *>(type);

    if (!basicType)
    {
        // Could be pointer type, in which case no conversion is needed
        return;
    }

    Types t = TypeChecker::runIntegerPromotions(basicType->type_);

    // sext is not allowed for i1 (because 1 -> -1)
    // doesn't matter if we want to promote to signed int
    if (BasicType(t).isSigned() && val->getType()->isIntegerTy(1))
    {
        val = builder_->CreateSExt(val, getLLVMType(t));
    }
    else
    {
        val = builder_->CreateZExt(val, getLLVMType(t));
    }
}

} // namespace CodeGen
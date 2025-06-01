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

CodeGenModule::CodeGenModule(
    std::string sourceFile,
    std::string outputFile,
    NodeMap &nodeMap,
    StructMap &structMap,
    std::string targetTriple)
    : outputFile_(std::move(outputFile)), nodeMap_(nodeMap),
      structMap_(structMap), context_(std::make_unique<llvm::LLVMContext>()),
      builder_(std::make_unique<llvm::IRBuilder<>>(*context_)),
      module_(std::make_unique<llvm::Module>("Module", *context_))
{
    // Initialise all the targets for emitting object code
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    llvm::InitializeAllAsmParsers();

    if (targetTriple.empty())
    {
        targetTriple = llvm::sys::getDefaultTargetTriple();
    }

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
    targetMachine_ = target->createTargetMachine(
        targetTriple, CPU, features, opt, llvm::Reloc::Model::PIC_);

    module_->setDataLayout(targetMachine_->createDataLayout());
    module_->setTargetTriple(targetTriple);
    module_->setSourceFileName(sourceFile);
    module_->setModuleIdentifier(sourceFile);

    llvm::Triple triple = llvm::Triple(targetTriple);

    if (triple.isAArch64())
    {
        abi_ = std::make_unique<AArch64ABI>(*module_);
    }
    else if (triple.isX86())
    {
        abi_ = std::make_unique<X86_64ABI>(*module_);
    }
    else
    {
        llvm::errs() << "Warning: Target architecture not recognised, "
                        "defaulting to x86-64\n";
        abi_ = std::make_unique<X86_64ABI>(*module_);
    }
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

    if (targetMachine_->addPassesToEmitFile(passManager, os, nullptr, fileType))
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

void CodeGenModule::visit(const AbstractArrayDecl &node)
{
    // Intentionally left blank
}

void CodeGenModule::visit(const AbstractTypeDecl &node)
{
    // Intentionally left blank
}

void CodeGenModule::visit(const ArrayDecl &node)
{
    // Allocate memory for the array
    node.decl_->accept(*this);
}

void CodeGenModule::visit(const BasicTypeDecl &node)
{
    // Intentionally left blank
}

void CodeGenModule::visit(const CompoundTypeDecl &node)
{
    // For struct/enum/union definitions
    for (const auto &typeDecl : node.nodes_)
    {
        std::visit(
            [this](const auto &typeDecl) { typeDecl->accept(*this); },
            typeDecl);
    }
}

void CodeGenModule::visit(const DeclNode &node)
{
    // For struct/union definitions
    node.type_->accept(*this);

    // Allocate memory for the variable
    // Exception for typedefs, don't want to allocate "memory" for them
    if (node.initDeclList_ && !dynamic_cast<const Typedef *>(node.type_.get()))
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
    if (auto *enumType = dynamic_cast<const EnumType *>(nodeMap_[&node].get()))
    {
        for (const auto &member : enumType->consts_)
        {
            symbolTablePush(
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
    // Intentionally left blank
}

void CodeGenModule::visit(const FnDef &node)
{
    std::string fnName = node.decl_->getID();
    llvm::Function *fn = module_->getFunction(fnName);

    // Safe to do... this was checked in the TypeChecker
    const FnType *type = dynamic_cast<const FnType *>(nodeMap_[&node].get());
    llvm::Type *retType = getLLVMType(type->retType_.get());

    // If we haven't declared the function yet, create it
    if (!fn)
    {
        auto linkage = (type->linkage_ == Linkage::INTERNAL)
                           ? llvm::Function::InternalLinkage
                           : llvm::Function::ExternalLinkage;
        fn = createFunction(type, linkage, fnName);
    }

    llvm::BasicBlock *bb = llvm::BasicBlock::Create(*context_, "entry", fn);
    builder_->SetInsertPoint(bb);

    // Required, quick solution to scopes being inside compound statements
    // Works the same, but we have 1 extra scope that only contains the function
    // arguments
    pushScope();

    // Make distinction of RAW paramType (before ABI modifications)
    std::vector<llvm::Type *> rawParamTypes = getParamTypes(type);
    auto fnParams = abi_->getFunctionParams(retType, rawParamTypes);
    unsigned argPtr = fnParams.structReturnInMemory ? 1 : 0;

    for (size_t j = 0; j < type->params_->size(); j++)
    {
        std::string paramName = type->getParamName(j);
        llvm::Type *rawParamType = rawParamTypes.at(j);

        if (rawParamType->isStructTy() &&
            fn->getArg(argPtr)->getType()->isPointerTy())
        {
            symbolTablePush(paramName, fn->getArg(argPtr));
            argPtr++;
        }
        else
        {
            llvm::AllocaInst *allocaInst;

            if (rawParamType->isStructTy() &&
                !fn->getArg(argPtr)->getType()->isPointerTy())
            {
                std::vector<llvm::Type *> tys =
                    fnParams.paramTypes.at(j + fnParams.structReturnInMemory);

                // AArch64 uses ArrayType for HFA e.g. [2 x i64]
                // x86-64 uses exploded structs e.g. [ i64, i64 ], pack them
                auto *assignedType =
                    (tys[0]->isArrayTy())
                        ? tys[0]
                        : llvm::StructType::get(*context_, tys);

                llvm::AllocaInst *tempAlloca =
                    createAlignedAlloca(assignedType);
                allocaInst = createAlignedAlloca(rawParamType, paramName);

                // Copy the params to an alloca of the (anonymous) struct type
                for (size_t i = 0; i < tys.size(); i++)
                {
                    llvm::Value *gep = builder_->CreateGEP(
                        assignedType,
                        tempAlloca,
                        {builder_->getInt32(0), builder_->getInt32(i)});
                    builder_->CreateStore(fn->getArg(argPtr), gep);
                    argPtr++;
                }

                // Copy the alloca to the actual alloca
                builder_->CreateMemCpy(
                    allocaInst,
                    getAlign(rawParamType),
                    tempAlloca,
                    getAlign(assignedType),
                    module_->getDataLayout().getTypeAllocSize(rawParamType));
            }
            else
            {
                allocaInst = createAlignedAlloca(rawParamType, paramName);
                builder_->CreateStore(fn->getArg(argPtr), allocaInst);
                argPtr++;
            }

            symbolTablePush(paramName, allocaInst);
        }
    }

    // Generate the function body
    isGlobal_ = false;
    node.body_->accept(*this);
    isGlobal_ = true;

    // If the function does not have a return statement, add one
    if (!builder_->GetInsertBlock()->getTerminator())
    {
        if (fn->getReturnType()->isVoidTy())
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

    llvm::verifyFunction(*fn, &llvm::errs());
}

void CodeGenModule::visit(const InitDecl &node)
{
    const auto *ty = nodeMap_[&node].get();
    llvm::Type *type = getLLVMType(ty);
    bool hasStatic = ty->linkage_ == Linkage::INTERNAL;
    bool hasExtern = ty->linkage_ == Linkage::EXTERNAL;
    auto linkage = hasStatic ? llvm::Function::InternalLinkage
                             : llvm::Function::ExternalLinkage;

    if (type->isFunctionTy())
    {
        // Global or local function declaration
        llvm::Function *fn = module_->getFunction(node.getID());

        if (!fn)
        {
            // Function declaration
            fn = createFunction(
                dynamic_cast<const FnType *>(ty), linkage, node.getID());
        }
        else if (
            hasExtern && fn->getLinkage() != llvm::GlobalValue::InternalLinkage)
        {
            // Function redeclaration, but extern overwrites everything EXCEPT
            // internal linkage
            fn->setLinkage(llvm::Function::ExternalLinkage);
        }

        // Label the function arguments
        node.decl_->accept(*this);
    }
    else if (isGlobal_ || hasStatic)
    {
        // Global or local static variable
        // C99 6.7.8.10: Essentially everything with a "static storage duration"
        // is initialized to 0 (unless specified otherwise). So the tentative
        // definition acts as an actual definition here.
        llvm::Constant *init = nullptr;

        if (node.init_)
        {
            init = visitAsConstant(*node.init_.get(), ty);
        }
        else if (!hasExtern)
        {
            // Zero-initialize the variable
            init = llvm::Constant::getNullValue(type);
        }

        std::string name =
            (isGlobal_) ? node.getID() : getLocalStaticName(node.getID());
        llvm::GlobalVariable *gb = module_->getGlobalVariable(name, true);
        if (gb)
        {
            if (node.init_ || !gb->hasInitializer())
            {
                // Definition
                gb->setInitializer(init);
                // Linkage can't be changed. If static keyword is found after
                // external linkage, that is UB
            }
            // else: redeclaration, do nothing
        }
        else
        {
            // Declaration
            gb = createAlignedGlobalVariable(
                *module_,
                type,
                /* isConstant */ false,
                linkage,
                init,
                name);
        }

        // Local static variables searched up by ID not name
        symbolTablePush(node.getID(), gb);
    }
    else if (hasExtern)
    {
        // Local extern variable
        llvm::GlobalVariable *gb =
            module_->getGlobalVariable(node.getID(), true);
        if (!gb)
        {
            // Variable is already declared somewhere else
            gb = createAlignedGlobalVariable(
                *module_,
                type,
                /* isConstant */ false,
                llvm::GlobalValue::ExternalLinkage,
                /* Initializer */ nullptr,
                node.getID());
        }
        symbolTablePush(node.getID(), gb);
        // An initializer is not allowed for an extern declaration
    }
    else
    {
        // Local non-static, non-extern variable
        // Allocate memory for the variable
        llvm::AllocaInst *alloca = createAlignedAlloca(type, node.getID());

        symbolTablePush(node.getID(), alloca);

        if (node.init_)
        {
            if (type->isAggregateType())
            {
                try
                {
                    // Try to initialize as a constant (generates less code)
                    llvm::Constant *cons = visitAsConstant(*node.init_, ty);
                    llvm::GlobalVariable *gb = createAlignedGlobalVariable(
                        *module_,
                        type,
                        /* isConstant */ true,
                        llvm::GlobalValue::InternalLinkage,
                        cons,
                        "__const." + getLocalStaticName(node.getID()));
                    builder_->CreateMemCpy(
                        alloca,
                        getAlign(type),
                        gb,
                        getAlign(type),
                        module_->getDataLayout().getTypeAllocSize(type));
                }
                catch (...)
                {
                    // Array initialization (placed here to init only once)
                    builder_->CreateMemSet(
                        alloca,
                        llvm::ConstantInt::get(
                            llvm::Type::getInt8Ty(*context_), 0, false),
                        module_->getDataLayout().getTypeAllocSize(type),
                        llvm::MaybeAlign());
                    visitAsStore(*node.init_, alloca, ty);
                }
            }
            else
            {
                visitAsStore(*node.init_, alloca, ty);
            }
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
    // Intentionally left blank
}

void CodeGenModule::visit(const ParamList &node)
{
    // Intentionally left blank
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

    // TypeChecker does semantic analysis, so all structs are complete
    // If not then, it's an opaque struct

    // All information is available in the typeMap
    const StructType *type =
        dynamic_cast<const StructType *>(nodeMap_[&node].get());

    std::string name = "struct." + node.name_;
    if (structIDs_.find(name) == structIDs_.end())
    {
        structIDs_[name] = {type->getID()};
    }
    else
    {
        auto it = std::find(
            structIDs_[name].begin(), structIDs_[name].end(), type->getID());
        if (it != structIDs_[name].end())
        {
            // Struct already defined
            return;
        }

        // New ID - add to the list
        structIDs_[name].push_back(type->getID());
        name += "." + std::to_string(structIDs_[name].size() - 1);
    }

    if (auto &params = structMap_[type->getID()])
    {
        std::vector<llvm::Type *> memberTypes;
        for (const auto &member : params->types_)
        {
            memberTypes.push_back(getLLVMType(member.second.get()));
        }

        // Set the type in LLVM
        llvm::StructType *structType =
            llvm::StructType::create(*context_, name);
        structType->setBody(memberTypes);
    }
    else
    {
        // Opaque struct
        llvm::StructType::create(*context_, name);
    }
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

void CodeGenModule::visit(const TypeModifier &node)
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

    // Weird semantics of C... `a[5] == 5[a]`
    const Expr *arrNode = node.arr_.get();
    const Expr *indexNode = node.index_.get();
    if (dynamic_cast<const BasicType *>(nodeMap_[arrNode].get()))
    {
        std::swap(arrNode, indexNode);
    }

    llvm::Value *index = visitAsRValue(*indexNode);
    llvm::Type *elementType = getLLVMType(&node);
    llvm::Value *arrayPtr;

    if (getLLVMType(arrNode)->isPointerTy())
    {
        // Load the pointer first
        llvm::Value *arr = visitAsRValue(*arrNode);
        arrayPtr = builder_->CreateInBoundsGEP(elementType, arr, index, "gep");
    }
    else
    {
        llvm::Value *arr = visitAsLValue(*arrNode);
        llvm::Value *zero =
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
        arrayPtr = builder_->CreateInBoundsGEP(
            getLLVMType(arrNode), arr, {zero, index}, "gep");
    }

    if (valueCategory == ValueCategory::LVALUE || elementType->isStructTy())
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
        // throw std::runtime_error("Assignment to LValue not supported");
    }

    using Op = Assignment::Op;

    auto *lhsType = nodeMap_[node.lhs_.get()].get();
    llvm::Value *lhs = visitAsLValue(*node.lhs_);
    bool isFloatTy = lhs->getType()->isFloatingPointTy();
    bool isSigned = false;
    if (auto *basicType = dynamic_cast<const BasicType *>(lhsType))
    {
        isSigned = basicType->isSigned();
    }

    // Handle simple assignment separately
    if (node.op_ == Assignment::Op::ASSIGN)
    {
        // This has to be compatible with structs (e.g. struct x = y;) which
        // uses memcpy(), not store
        currentValue_ = visitAsStore(*node.rhs_, lhs, lhsType);
        return;
    }

    llvm::Value *lhsVal = visitAsRValue(*node.lhs_);
    llvm::Value *rhs = visitAsCastedRValue(*node.rhs_, lhsType);
    llvm::Value *expr;

    switch (node.op_)
    {
    case Op::ASSIGN:
        // Handled separately
        return;
    case Op::MUL_ASSIGN:
        expr = (isFloatTy) ? builder_->CreateFMul(lhsVal, rhs, "mul")
                           : builder_->CreateMul(lhsVal, rhs, "mul");
        break;
    case Op::DIV_ASSIGN:
        expr = (isFloatTy)  ? builder_->CreateFDiv(lhsVal, rhs, "div")
               : (isSigned) ? builder_->CreateSDiv(lhsVal, rhs, "div")
                            : builder_->CreateUDiv(lhsVal, rhs, "div");
        break;
    case Op::MOD_ASSIGN:
        expr = (isFloatTy)  ? builder_->CreateFRem(lhsVal, rhs, "mod")
               : (isSigned) ? builder_->CreateSRem(lhsVal, rhs, "mod")
                            : builder_->CreateURem(lhsVal, rhs, "mod");
        break;
    case Op::ADD_ASSIGN:
        expr = (isFloatTy) ? builder_->CreateFAdd(lhsVal, rhs, "add")
                           : builder_->CreateAdd(lhsVal, rhs, "add");
        break;
    case Op::SUB_ASSIGN:
        expr = (isFloatTy) ? builder_->CreateFSub(lhsVal, rhs, "sub")
                           : builder_->CreateSub(lhsVal, rhs, "sub");
        break;
    case Op::LEFT_ASSIGN:
        expr = builder_->CreateShl(lhsVal, rhs, "shl");
        break;
    case Op::RIGHT_ASSIGN:
        expr = (isSigned) ? builder_->CreateAShr(lhsVal, rhs, "shr")
                          : builder_->CreateLShr(lhsVal, rhs, "shr");
        break;
    case Op::AND_ASSIGN:
        expr = builder_->CreateAnd(lhsVal, rhs, "and");
        break;
    case Op::XOR_ASSIGN:
        expr = builder_->CreateXor(lhsVal, rhs, "xor");
        break;
    case Op::OR_ASSIGN:
        expr = builder_->CreateOr(lhsVal, rhs, "or");
        break;
    }

    builder_->CreateStore(expr, lhs);
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

    auto *lhsType = nodeMap_[node.lhs_.get()].get();
    auto *rhsType = nodeMap_[node.rhs_.get()].get();

    if (lhsType->isArrayOrPtrTy() || rhsType->isArrayOrPtrTy())
    {
        visitPtrOp(node);
        return;
    }

    llvm::Value *lhs = visitAsRValue(*node.lhs_);
    llvm::Value *rhs = visitAsRValue(*node.rhs_);
    lhs = runUsualArithmeticConversions(lhsType, rhsType, lhs);
    rhs = runUsualArithmeticConversions(rhsType, lhsType, rhs);

    bool isFloat = lhs->getType()->isFloatingPointTy();
    bool isSigned =
        BasicType(getArithmeticConversionType(lhsType, rhsType)).isSigned();

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
        currentValue_ =
            (isFloat) ? builder_->CreateFDiv(lhs, rhs, "div")
                      : ((isSigned) ? builder_->CreateSDiv(lhs, rhs, "div")
                                    : builder_->CreateUDiv(lhs, rhs, "div"));
        break;
    case Op::MOD:
        currentValue_ =
            (isFloat) ? builder_->CreateFRem(lhs, rhs, "mod")
                      : ((isSigned) ? builder_->CreateSRem(lhs, rhs, "mod")
                                    : builder_->CreateURem(lhs, rhs, "mod"));
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
        currentValue_ = (isSigned) ? builder_->CreateAShr(lhs, rhs, "shr")
                                   : builder_->CreateLShr(lhs, rhs, "shr");
        break;
    case Op::EQ:
        currentValue_ = (isFloat) ? builder_->CreateFCmpOEQ(lhs, rhs, "eq")
                                  : builder_->CreateICmpEQ(lhs, rhs, "eq");
        break;
    case Op::NE:
        currentValue_ = (isFloat) ? builder_->CreateFCmpONE(lhs, rhs, "ne")
                                  : builder_->CreateICmpNE(lhs, rhs, "ne");
        break;
    case Op::LT:
        currentValue_ = (isFloat)    ? builder_->CreateFCmpOLT(lhs, rhs, "lt")
                        : (isSigned) ? builder_->CreateICmpSLT(lhs, rhs, "lt")
                                     : builder_->CreateICmpULT(lhs, rhs, "lt");
        break;
    case Op::GT:
        currentValue_ = (isFloat)    ? builder_->CreateFCmpOGT(lhs, rhs, "gt")
                        : (isSigned) ? builder_->CreateICmpSGT(lhs, rhs, "gt")
                                     : builder_->CreateICmpUGT(lhs, rhs, "gt");
        break;
    case Op::LE:
        currentValue_ = (isFloat)    ? builder_->CreateFCmpOLE(lhs, rhs, "le")
                        : (isSigned) ? builder_->CreateICmpSLE(lhs, rhs, "le")
                                     : builder_->CreateICmpULE(lhs, rhs, "le");
        break;
    case Op::GE:
        currentValue_ = (isFloat)    ? builder_->CreateFCmpOGE(lhs, rhs, "ge")
                        : (isSigned) ? builder_->CreateICmpSGE(lhs, rhs, "ge")
                                     : builder_->CreateICmpUGE(lhs, rhs, "ge");
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

    // LHS
    llvm::Value *lhs = visitAsRValue(*node.lhs_);
    llvm::Value *lhsBool = isNotZero(lhs);
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
    llvm::Value *rhsBool = isNotZero(rhs);
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

void CodeGenModule::visitPtrOp(const BinaryOp &node)
{
    using Op = BinaryOp::Op;

    // Array types are loaded in raw (i.e. as pointers)
    llvm::Value *lhs = (getLLVMType(node.lhs_.get())->isArrayTy())
                           ? visitAsLValue(*node.lhs_)
                           : visitAsRValue(*node.lhs_);
    llvm::Value *rhs = (getLLVMType(node.rhs_.get())->isArrayTy())
                           ? visitAsLValue(*node.rhs_)
                           : visitAsRValue(*node.rhs_);

    // Helper function for comparisons
    auto castComparison = [this](llvm::Value *&lhs, llvm::Value *&rhs) -> void
    {
        if (lhs->getType()->isIntegerTy())
        {
            lhs = builder_->CreateIntToPtr(lhs, rhs->getType(), "inttoptr");
        }
        else if (rhs->getType()->isIntegerTy())
        {
            rhs = builder_->CreateIntToPtr(rhs, lhs->getType(), "inttoptr");
        }
    };

    switch (node.op_)
    {
    case Op::ADD:
        // Pointer arithmetic (getelementptr inbounds)
        if (rhs->getType()->isPointerTy())
        {
            // Pointer becomes lhs
            std::swap(lhs, rhs);
        }

        currentValue_ = builder_->CreateInBoundsGEP(
            getPointerElementType(&node), lhs, rhs, "add");

        break;
    case Op::SUB:
        // LHS must be a pointer (otherwise type error)
        // RHS must be the same type (otherwise UB)
        // Pointer subtraction
        if (rhs->getType()->isPointerTy())
        {
            llvm::Type *ptrType = getPointerElementType(node.lhs_.get());
            currentValue_ = builder_->CreatePtrDiff(ptrType, lhs, rhs, "sub");
        }
        else
        {
            // Pointer arithmetic
            rhs = builder_->CreateMul(
                rhs, llvm::ConstantInt::get(rhs->getType(), -1), "neg");
            currentValue_ = builder_->CreateInBoundsGEP(
                getPointerElementType(&node), lhs, rhs, "sub");
        }
        break;
    case Op::EQ:
        castComparison(lhs, rhs);
        currentValue_ = builder_->CreateICmpEQ(lhs, rhs, "eq");
        break;
    case Op::NE:
        castComparison(lhs, rhs);
        currentValue_ = builder_->CreateICmpNE(lhs, rhs, "ne");
        break;
    case Op::LT:
        castComparison(lhs, rhs);
        currentValue_ = builder_->CreateICmpULT(lhs, rhs, "lt");
        break;
    case Op::GT:
        castComparison(lhs, rhs);
        currentValue_ = builder_->CreateICmpUGT(lhs, rhs, "gt");
        break;
    case Op::LE:
        castComparison(lhs, rhs);
        currentValue_ = builder_->CreateICmpULE(lhs, rhs, "le");
        break;
    case Op::GE:
        castComparison(lhs, rhs);
        currentValue_ = builder_->CreateICmpUGE(lhs, rhs, "ge");
        break;
    case Op::MUL:
    case Op::DIV:
    case Op::MOD:
    case Op::AND:
    case Op::OR:
    case Op::XOR:
    case Op::SHL:
    case Op::SHR:
        // Undefined behaviour
    case Op::LAND:
    case Op::LOR:
        // Handled in a different function
        break;
    }
}

void CodeGenModule::visit(const Cast &node)
{
    if (valueCategory_ == ValueCategory::LVALUE)
    {
        throw std::runtime_error("Cast to LValue not supported");
    }

    auto *expectedType = nodeMap_[node.type_.get()].get();

    currentValue_ = visitAsCastedRValue(*node.expr_, expectedType);
}

void CodeGenModule::visit(const Constant &node)
{
    if (valueCategory_ != ValueCategory::RVALUE)
    {
        throw std::runtime_error("Constant to LValue not supported");
    }

    auto *ty = nodeMap_[&node].get();
    auto basicType = dynamic_cast<const BasicType *>(ty);
    llvm::Type *type = getLLVMType(ty);

    if (type->isIntegerTy(8))
    {
        // Gets the 1 or 2 characters between the single quotes
        currentValue_ = llvm::ConstantInt::get(type, node.getChar());
    }
    else if (type->isFloatingPointTy())
    {
        currentValue_ = llvm::ConstantFP::get(type, *node.eval().getDouble());
    }
    else if (!basicType->isSigned())
    {
        currentValue_ = llvm::ConstantInt::get(type, *node.eval().getUInt());
    }
    else
    {
        currentValue_ = llvm::ConstantInt::get(type, *node.eval().getInt());
    }
}

void CodeGenModule::visit(const FnCall &node)
{
    if (valueCategory_ != ValueCategory::RVALUE)
    {
        // throw std::runtime_error("FnCall to LValue not supported");
    }

    llvm::Function *fn = visitAsFnDesignator(*node.fn_);
    const FnType *fnType =
        dynamic_cast<const FnType *>(nodeMap_[node.fn_.get()].get());
    auto paramTypes = getParamTypes(fnType);
    llvm::Type *originalRetType = getLLVMType(fnType->retType_.get());
    auto fnParams = abi_->getFunctionParams(originalRetType, paramTypes);
    std::vector<llvm::Value *> args;

    if (fnParams.structReturnInMemory)
    {
        llvm::AllocaInst *allocaInst = createAlignedAlloca(originalRetType);
        args.push_back(allocaInst);
    }

    // Refers to the ABI for the calling convention
    if (node.args_)
    {
        for (size_t i = 0; i < node.args_->nodes_.size(); i++)
        {
            auto &arg = node.args_->nodes_[i];
            std::visit(
                [&](const auto &arg)
                {
                    auto *expectedType = dynamic_cast<const ParamType *>(
                                             nodeMap_[node.args_.get()].get())
                                             ->at(i);
                    auto *ty = getLLVMType(expectedType);

                    std::vector<llvm::Type *> types = fnParams.paramTypes.at(
                        i + fnParams.structReturnInMemory);

                    if (ty->isStructTy())
                    {
                        llvm::Value *argL = visitAsLValue(*arg);

                        if (types.size() > 1)
                        {
                            // Struct with 2 elements
                            auto *accessTy =
                                llvm::StructType::get(*context_, types);
                            llvm::AllocaInst *tempAlloca =
                                createAlignedAlloca(accessTy);

                            builder_->CreateMemCpy(
                                tempAlloca,
                                getAlign(tempAlloca->getType()),
                                argL,
                                getAlign(argL->getType()),
                                module_->getDataLayout().getTypeAllocSize(ty));

                            for (size_t j = 0; j < types.size(); j++)
                            {
                                llvm::Value *gep = builder_->CreateInBoundsGEP(
                                    accessTy,
                                    tempAlloca,
                                    {builder_->getInt32(0),
                                     builder_->getInt32(j)});
                                args.push_back(
                                    builder_->CreateLoad(types[j], gep));
                            }
                        }
                        else if (types[0]->isArrayTy())
                        {
                            auto zero = builder_->getInt32(0);
                            llvm::Value *gep = builder_->CreateInBoundsGEP(
                                ty, argL, {zero, zero});
                            args.push_back(builder_->CreateLoad(types[0], gep));
                        }
                        else if (fn->getArg(args.size())->hasByValAttr())
                        {
                            // Struct passed by value
                            // Global variables must be copied to an alloca
                            if (auto *global =
                                    llvm::dyn_cast<llvm::GlobalVariable>(argL))
                            {
                                llvm::AllocaInst *tempAlloca =
                                    createAlignedAlloca(global->getValueType());
                                builder_->CreateMemCpy(
                                    tempAlloca,
                                    getAlign(tempAlloca->getType()),
                                    global,
                                    getAlign(global->getValueType()),
                                    module_->getDataLayout().getTypeAllocSize(
                                        global->getValueType()));
                                args.push_back(tempAlloca);
                            }
                            else
                            {
                                args.push_back(argL);
                            }
                        }
                        else if (fn->getArg(args.size())
                                     ->getType()
                                     ->isPointerTy())
                        {
                            // Struct passed by value, but not explictly using
                            // ByVal
                            llvm::AllocaInst *tempAlloca =
                                createAlignedAlloca(ty);
                            builder_->CreateMemCpy(
                                tempAlloca,
                                getAlign(tempAlloca->getType()),
                                argL,
                                getAlign(argL->getType()),
                                module_->getDataLayout().getTypeAllocSize(ty));
                            args.push_back(tempAlloca);
                        }
                        else
                        {
                            // Struct with 1 element
                            args.push_back(
                                builder_->CreateLoad(types[0], argL));
                        }
                    }
                    else
                    {
                        // Normal case
                        args.push_back(visitAsCastedRValue(*arg, expectedType));
                    }
                },
                arg);
        }
    }

    auto *callInst = builder_->CreateCall(fn, args);

    if (fnParams.structReturnInMemory)
    {
        callInst->addParamAttr(0, llvm::Attribute::DeadOnUnwind);
        callInst->addParamAttr(0, llvm::Attribute::Writable);
        callInst->addParamAttr(
            0,
            llvm::Attribute::getWithStructRetType(
                *context_, getLLVMType(&node)));

        currentValue_ = callInst->getArgOperand(0);
    }
    else
    {
        currentValue_ = callInst;
    }
}

void CodeGenModule::visit(const Identifier &node)
{
    Symbol symbol = symbolTableLookup(node.getID());

    if (valueCategory_ == ValueCategory::LVALUE)
    {
        if (auto **alloca = std::get_if<llvm::AllocaInst *>(&symbol))
        {
            currentValue_ = *alloca;

            // Only for automatic arrays, for alignment purposes
            if ((*alloca)->getAllocatedType()->isArrayTy())
            {
                auto zero = builder_->getInt32(0);
                currentValue_ = builder_->CreateInBoundsGEP(
                    (*alloca)->getAllocatedType(), currentValue_, {zero, zero});
            }
        }
        else if (auto **arg = std::get_if<llvm::Argument *>(&symbol))
        {
            currentValue_ = *arg;
        }
        else if (auto **global = std::get_if<llvm::GlobalVariable *>(&symbol))
        {
            currentValue_ = *global;
        }
        else
        {
            throw std::runtime_error("Unknown identifier: " + node.getID());
        }
    }
    else if (valueCategory_ == ValueCategory::RVALUE)
    {
        // Special handling needed for arrays, they decay to pointers
        // Structs cannot be passed by value
        llvm::Type *type = getLLVMType(&node);
        if (type->isAggregateType())
        {
            currentValue_ = visitAsLValue(node);
        }
        else if (auto **alloca = std::get_if<llvm::AllocaInst *>(&symbol))
        {
            currentValue_ = builder_->CreateLoad(
                (*alloca)->getAllocatedType(), *alloca, node.getID());
        }
        else if (auto **arg = std::get_if<llvm::Argument *>(&symbol))
        {
            // For byvals, no need to load
            currentValue_ = *arg;
        }
        else if (auto **constant = std::get_if<llvm::Constant *>(&symbol))
        {
            currentValue_ = *constant;
        }
        else if (auto **global = std::get_if<llvm::GlobalVariable *>(&symbol))
        {
            currentValue_ = builder_->CreateLoad(
                (*global)->getValueType(), *global, node.getID());
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

void CodeGenModule::visit(const Init &node)
{
    if (valueCategory_ != ValueCategory::RVALUE)
    {
        // throw std::runtime_error("Init to LValue not supported");
    }

    if (currentStore_)
    {
        // Scenario 1. visitAsStore
        node.expr_->accept(*this);
    }
    else
    {
        // Scenario 2. visitAsConstant
        if (auto *initList = dynamic_cast<const InitList *>(node.expr_.get()))
        {
            initList->accept(*this);
        }
        else
        {
            // Use .value() as it could throw an exception
            auto *expectedType = getLLVMType(currentExpectedType_);
            auto *basicType =
                dynamic_cast<const BasicType *>(currentExpectedType_);

            if (auto *arrType =
                    dynamic_cast<const ArrayType *>(currentExpectedType_))
            {
                // String literal (doesn't go through InitList)
                int strlen = arrType->size_;
                std::vector<llvm::Constant *> values;
                std::string str = node.expr_->eval().getString().value();
                for (size_t i = 0; i < strlen; i++)
                {
                    if (i < str.size())
                    {
                        values.push_back(
                            llvm::ConstantInt::get(
                                llvm::Type::getInt8Ty(*context_), str[i]));
                    }
                    else
                    {
                        values.push_back(
                            llvm::ConstantInt::get(
                                llvm::Type::getInt8Ty(*context_), 0));
                    }
                }
                currentValue_ = llvm::ConstantArray::get(
                    llvm::ArrayType::get(
                        llvm::Type::getInt8Ty(*context_), strlen),
                    values);
            }
            else if (currentExpectedType_->isPtrTy())
            {
                if (node.expr_->eval().is<std::string>())
                {
                    // String literal (decays to a pointer)
                    std::string value = node.expr_->eval().getString().value();
                    currentValue_ = builder_->CreateGlobalString(
                        value, "", 0U, module_.get());
                }
                else if (node.expr_->eval().getUInt().value() == 0)
                {
                    currentValue_ = llvm::ConstantPointerNull::get(
                        static_cast<llvm::PointerType *>(expectedType));
                }
                else
                {
                    throw std::runtime_error("Expected a null pointer");
                }
            }
            else if (!basicType)
            {
                // OK, this will be caught in a try-catch block in InitDecl
                throw std::runtime_error("Expected a basic type");
            }
            else if (expectedType->isFloatingPointTy())
            {
                currentValue_ = llvm::ConstantFP::get(
                    expectedType, node.expr_->eval().getDouble().value());
            }
            else if (basicType->isSigned())
            {
                currentValue_ = llvm::ConstantInt::get(
                    expectedType, node.expr_->eval().getInt().value());
            }
            else
            {
                currentValue_ = llvm::ConstantInt::get(
                    expectedType, node.expr_->eval().getUInt().value());
            }
        }
    }
}

void CodeGenModule::visit(const InitList &node)
{
    if (valueCategory_ != ValueCategory::RVALUE)
    {
        // throw std::runtime_error("InitList to LValue not supported");
    }

    if (currentStore_)
    {
        // Scenario 1. visitAsStore
        visitRecursiveStore(
            node, std::vector<llvm::Value *>{builder_->getInt32(0)});
        currentValue_ = nullptr;
    }
    else
    {
        // Scenario 2. visitAsConstant
        currentValue_ = visitRecursiveConst(node);
    }
}

void CodeGenModule::visitRecursiveStore(
    const InitList &node,
    std::vector<llvm::Value *> indices)
{
    for (size_t i = 0; i < node.nodes_.size(); i++)
    {
        const BaseType *newType;
        if (auto *arrType =
                dynamic_cast<const ArrayType *>(currentExpectedType_))
        {
            newType = arrType->type_.get(); // Change the expected type
        }
        else if (
            auto *structType =
                dynamic_cast<const StructType *>(currentExpectedType_))
        {
            // Some long indirection going on but whatever...
            newType =
                structMap_.at(structType->getID())->types_[i].second.get();
        }

        std::visit(
            [&](const Ptr<Init> &n)
            {
                // Push the index, pop on exit
                llvm::Value *index = llvm::ConstantInt::get(
                    llvm::Type::getInt32Ty(*context_), i);
                ScopeGuard sg(indices, index);
                llvm::Value *gep = builder_->CreateInBoundsGEP(
                    getLLVMType(currentExpectedType_),
                    currentStore_,
                    indices,
                    "gep");

                if (auto *initList = dynamic_cast<const InitList *>(n.get()))
                {
                    // Recursive case
                    ScopeGuard sg2(currentExpectedType_, newType);
                    ScopeGuard sg3(currentStore_, gep);
                    visitRecursiveStore(*initList, indices);
                }
                else
                {
                    // Base case (ScopeGuards inserted in function)
                    visitAsStore(*n, gep, newType);
                }
            },
            node.nodes_[i]);
    }
}

llvm::Constant *CodeGenModule::visitRecursiveConst(const InitList &node)
{
    // Assume it's an array
    std::vector<llvm::Constant *> values;
    llvm::Type *type = getLLVMType(currentExpectedType_);

    for (size_t i = 0; i < node.nodes_.size(); i++)
    {
        const BaseType *newType;
        if (auto *arrType =
                dynamic_cast<const ArrayType *>(currentExpectedType_))
        {
            newType = arrType->type_.get(); // Change the expected type
        }
        else if (
            auto *structType =
                dynamic_cast<const StructType *>(currentExpectedType_))
        {
            // Some long indirection going on but whatever...
            newType =
                structMap_.at(structType->getID())->types_[i].second.get();
        }
        ScopeGuard sg(currentExpectedType_, newType);

        std::visit(
            [&](const auto &n)
            {
                if (auto *initList = dynamic_cast<const InitList *>(n.get()))
                {
                    // Recursive case
                    llvm::Constant *val = visitRecursiveConst(*initList);
                    values.push_back(val);
                }
                else
                {
                    // Base case
                    llvm::Constant *val = visitAsConstant(*n, newType);
                    values.push_back(val);
                }
            },
            node.nodes_[i]);
    }

    // C99 6.7.8.21
    // Essentially, if the array/string literal/struct is not fully initialized,
    // the rest of the elements are initialized like static variables, i.e. 0

    if (type->isArrayTy())
    {
        // Fill the array with zeroes
        while (values.size() < type->getArrayNumElements())
        {
            values.push_back(
                llvm::Constant::getNullValue(
                    static_cast<llvm::ArrayType *>(type)->getElementType()));
        }

        return llvm::ConstantArray::get(
            static_cast<llvm::ArrayType *>(type), values);
    }

    for (size_t i = values.size(); i < type->getStructNumElements(); i++)
    {
        values.push_back(
            llvm::Constant::getNullValue(
                static_cast<llvm::StructType *>(type)->getElementType(i)));
    }

    return llvm::ConstantStruct::get(
        static_cast<llvm::StructType *>(type), values);
}

void CodeGenModule::visit(const Paren &node)
{
    // Intentionally don't use the LValue/RValue handling
    // This method is "blind" to the value category, it passes it down
    if (node.expr_)
    {
        node.expr_->accept(*this);
    }
    else
    {
        node.decl_->accept(*this);
    }
}

void CodeGenModule::visit(const SizeOf &node)
{
    if (valueCategory_ != ValueCategory::RVALUE)
    {
        throw std::runtime_error("SizeOf to LValue not supported");
    }

    llvm::Type *type = (node.expr_) ? getLLVMType(node.expr_.get())
                                    : getLLVMType(node.type_.get());

    currentValue_ = llvm::ConstantInt::get(
        llvm::Type::getInt64Ty(*context_),
        module_->getDataLayout().getTypeAllocSize(type));
}

void CodeGenModule::visit(const StringLiteral &node)
{
    // C99 6.5.1: A string literal is a primary expression. It is an lvalue with
    // type as detailed in 6.4.5.
    llvm::GlobalVariable *str =
        builder_->CreateGlobalString(node.value_, "", 0U, module_.get());
    llvm::Value *zero =
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
    currentValue_ = builder_->CreateInBoundsGEP(
        str->getValueType(), str, {zero, zero}, "str");
}

void CodeGenModule::visit(const StructAccess &node)
{
    auto valueCategory = valueCategory_;

    llvm::Value *structPtr = visitAsLValue(*node.expr_);
    auto structType =
        dynamic_cast<const StructType *>(nodeMap_[node.expr_.get()].get());
    auto index =
        structMap_.at(structType->getID())->getMemberIndex(node.member_);
    llvm::Value *indices[] = {
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0),
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), index)};

    llvm::Value *memberPtr = builder_->CreateInBoundsGEP(
        getLLVMType(structType), structPtr, indices, "gep");

    if (valueCategory == ValueCategory::LVALUE ||
        getLLVMType(&node)->isStructTy())
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
    // Equivalent to (*expr).member
    auto valueCategory = valueCategory_;

    // Only difference is that we need to load the pointer
    llvm::Value *structPtr;
    const StructType *structType;
    llvm::Type *exprType = getLLVMType(node.expr_.get());
    auto *zero = builder_->getInt32(0);
    if (exprType->isArrayTy())
    {
        structPtr = visitAsLValue(*node.expr_);
        structPtr =
            builder_->CreateInBoundsGEP(exprType, structPtr, {zero, zero});
        auto arrayType =
            dynamic_cast<const ArrayType *>(nodeMap_[node.expr_.get()].get());
        structType = dynamic_cast<const StructType *>(arrayType->type_.get());
    }
    else
    {
        // Normal pointer type
        structPtr = visitAsRValue(*node.expr_);
        auto ptrType =
            dynamic_cast<const PtrType *>(nodeMap_[node.expr_.get()].get());
        structType = dynamic_cast<const StructType *>(ptrType->type_.get());
    }
    auto index =
        structMap_.at(structType->getID())->getMemberIndex(node.member_);
    llvm::Value *indices[] = {zero, builder_->getInt32(index)};

    llvm::Value *memberPtr = builder_->CreateInBoundsGEP(
        getLLVMType(structType), structPtr, indices, "gep");

    if (valueCategory == ValueCategory::LVALUE ||
        getLLVMType(&node)->isStructTy())
    {
        currentValue_ = memberPtr;
    }
    else
    {
        currentValue_ =
            builder_->CreateLoad(getLLVMType(&node), memberPtr, "load");
    }
}

void CodeGenModule::visit(const TernaryOp &node)
{
    if (valueCategory_ == ValueCategory::LVALUE)
    {
        // throw std::runtime_error("TernaryOp to LValue not supported");
    }

    llvm::Function *fn = getCurrentFunction();
    llvm::BasicBlock *lhsBB = llvm::BasicBlock::Create(*context_, "lhs", fn);
    llvm::BasicBlock *rhsBB = llvm::BasicBlock::Create(*context_, "rhs");
    llvm::BasicBlock *afterBB = llvm::BasicBlock::Create(*context_, "after");
    auto lhsType = nodeMap_[node.lhs_.get()].get();
    auto rhsType = nodeMap_[node.rhs_.get()].get();
    // Void types in the ternary operator (lhsType and rhsType) is valid in C
    bool isVoidType = getLLVMType(&node)->isVoidTy();

    llvm::Value *cond = visitAsRValue(*node.cond_);
    llvm::Value *condBool = isNotZero(cond);

    // True - evaluate LHS. False - evaluate RHS.
    builder_->CreateCondBr(condBool, lhsBB, rhsBB);

    // LHS
    builder_->SetInsertPoint(lhsBB);
    llvm::Value *lhs = visitAsRValue(*node.lhs_);
    if (!isVoidType)
    {
        lhs = runConversions(lhsType, rhsType, lhs);
    }
    builder_->CreateBr(afterBB);
    lhsBB = builder_->GetInsertBlock();

    // RHS
    fn->insert(fn->end(), rhsBB);
    builder_->SetInsertPoint(rhsBB);
    llvm::Value *rhs = visitAsRValue(*node.rhs_);
    if (!isVoidType)
    {
        rhs = runConversions(rhsType, lhsType, rhs);
    }
    builder_->CreateBr(afterBB);
    rhsBB = builder_->GetInsertBlock();

    // After
    fn->insert(fn->end(), afterBB);
    builder_->SetInsertPoint(afterBB);

    if (isVoidType)
    {
        currentValue_ = nullptr;
    }
    else
    {
        llvm::PHINode *phi = builder_->CreatePHI(lhs->getType(), 2, "phi");
        phi->addIncoming(lhs, lhsBB);
        phi->addIncoming(rhs, rhsBB);

        currentValue_ = phi;
    }
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
        llvm::Value *expr, *add, *sub;
        auto *expectedType = nodeMap_[&node].get();
        llvm::Type *type = getLLVMType(node.expr_.get());
        bool isFloat = type->isFloatingPointTy();
        llvm::Value *one = (isFloat) ? llvm::ConstantFP::get(type, 1.0)
                                     : builder_->getInt32(1);
        llvm::Value *zero = builder_->getInt32(0);

        switch (node.op_)
        {
        case UnaryOp::Op::ADDR:
            expr = visitAsLValue(*node.expr_);
            currentValue_ = expr;
            break;
        case UnaryOp::Op::DEREF:
            if (type->isArrayTy())
            {
                expr = visitAsLValue(*node.expr_);
                expr = builder_->CreateInBoundsGEP(
                    type, expr, {zero, zero}, "gep");
            }
            else
            {
                // Normal pointer type
                expr = visitAsRValue(*node.expr_);
            }

            // Need to find out the type again, because of opaque pointers
            if (getLLVMType(&node)->isStructTy())
            {
                // Loading structs are useless in LLVM (always used as pointers)
                currentValue_ = expr;
            }
            else
            {
                currentValue_ =
                    builder_->CreateLoad(getLLVMType(&node), expr, "deref");
            }
            break;
        case UnaryOp::Op::PLUS:
            // Integer promotions may apply
            expr = visitAsCastedRValue(*node.expr_, expectedType);
            currentValue_ = expr;
            break;
        case UnaryOp::Op::MINUS:
            expr = visitAsCastedRValue(*node.expr_, expectedType);
            currentValue_ = (isFloat) ? builder_->CreateFNeg(expr, "neg")
                                      : builder_->CreateNeg(expr, "neg");
            break;
        case UnaryOp::Op::NOT:
            expr = visitAsCastedRValue(*node.expr_, expectedType);
            currentValue_ = builder_->CreateNot(expr, "not");
            break;
        case UnaryOp::Op::LNOT:
            expr = visitAsRValue(*node.expr_);
            currentValue_ =
                (isFloat) ? builder_->CreateFCmpOEQ(
                                expr, llvm::ConstantFP::get(type, 0.0), "lnot")
                : (type->isPointerTy() || type->isArrayTy())
                    ? builder_->CreateICmpEQ(
                          expr,
                          llvm::ConstantPointerNull::get(
                              llvm::PointerType::get(*context_, 0)),
                          "lnot")
                    : builder_->CreateICmpEQ(
                          expr,
                          llvm::ConstantInt::get(type, 0, /* isSigned */ false),
                          "lnot");
            break;
        case UnaryOp::Op::POST_DEC:
            // `x--` is equivalent to `x = x - 1; return x;`
            expr = visitAsRValue(*node.expr_);
            if (expr->getType()->isPointerTy())
            {
                sub = builder_->CreateInBoundsGEP(
                    getPointerElementType(node.expr_.get()),
                    expr,
                    builder_->getInt32(-1),
                    "postdec");
            }
            else
            {
                sub = (isFloat) ? builder_->CreateFSub(expr, one, "postdec")
                                : builder_->CreateSub(expr, one, "postdec");
            }
            builder_->CreateStore(sub, visitAsLValue(*node.expr_));
            currentValue_ = expr;
            break;
        case UnaryOp::Op::POST_INC:
            // `x++` is equivalent to `x = x + 1; return x;`
            expr = visitAsRValue(*node.expr_);
            if (expr->getType()->isPointerTy())
            {
                add = builder_->CreateInBoundsGEP(
                    getPointerElementType(node.expr_.get()),
                    expr,
                    builder_->getInt32(1),
                    "postinc");
            }
            else
            {
                add = (isFloat) ? builder_->CreateFAdd(expr, one, "postinc")
                                : builder_->CreateAdd(expr, one, "postinc");
            }
            builder_->CreateStore(add, visitAsLValue(*node.expr_));
            currentValue_ = expr;
            break;
        case UnaryOp::Op::PRE_DEC:
            expr = visitAsRValue(*node.expr_);
            if (expr->getType()->isPointerTy())
            {
                currentValue_ = builder_->CreateInBoundsGEP(
                    getPointerElementType(node.expr_.get()),
                    expr,
                    builder_->getInt32(-1),
                    "predec");
            }
            else
            {
                currentValue_ = (isFloat)
                                    ? builder_->CreateFSub(expr, one, "predec")
                                    : builder_->CreateSub(expr, one, "predec");
            }
            break;
        case UnaryOp::Op::PRE_INC:
            expr = visitAsRValue(*node.expr_);
            if (expr->getType()->isPointerTy())
            {
                currentValue_ = builder_->CreateInBoundsGEP(
                    getPointerElementType(node.expr_.get()),
                    expr,
                    builder_->getInt32(1),
                    "preinc");
            }
            else
            {
                currentValue_ = (isFloat)
                                    ? builder_->CreateFAdd(expr, one, "preinc")
                                    : builder_->CreateAdd(expr, one, "preinc");
            }
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
        std::visit(
            [this](const auto &item)
            {
                // Trying to insert a statement after a terminator
                // e.g. int main() { return 0; int x; }
                // A case statement will insert a BasicBlock
                if (!builder_->GetInsertBlock()->getTerminator() ||
                    currentSwitch_)
                {
                    item->accept(*this);
                }
            },
            item);
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
        uint64_t value = *node.expr_->eval().getUInt();
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
    cond = isNotZero(cond);
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
        cond = isNotZero(cond);
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
    cond = isNotZero(cond);

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
    if (node.expr_)
    {
        auto *expectedType = nodeMap_[&node].get();
        auto *ty = getLLVMType(expectedType);

        // Return struct, in memory
        if (dynamic_cast<const StructType *>(expectedType))
        {
            if (getCurrentFunction()->getReturnType()->isVoidTy())
            {
                llvm::Value *dest = getCurrentFunction()->getArg(0);
                visitAsStore(*node.expr_, dest, expectedType);
                builder_->CreateRetVoid();
            }
            else
            {
                // Structs have to be copied before returning
                llvm::Value *dest = createAlignedAlloca(ty);
                visitAsStore(*node.expr_, dest, expectedType);
                llvm::Value *gep = builder_->CreateGEP(
                    ty, dest, {builder_->getInt32(0), builder_->getInt32(0)});
                builder_->CreateRet(builder_->CreateLoad(
                    getCurrentFunction()->getReturnType(), gep));
            }
        }
        else
        {
            llvm::Value *retValue =
                visitAsCastedRValue(*node.expr_, expectedType);
            builder_->CreateRet(retValue);
        }
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
    cond = isNotZero(cond);

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

llvm::AllocaInst *
CodeGenModule::createAlignedAlloca(llvm::Type *type, const llvm::Twine &name)
{
    llvm::AllocaInst *inst = builder_->CreateAlloca(type, nullptr, name);
    inst->setAlignment(getAlign(type));

    return inst;
}

llvm::GlobalVariable *CodeGenModule::createAlignedGlobalVariable(
    llvm::Module &M,
    llvm::Type *Ty,
    bool isConstant,
    llvm::GlobalVariable::LinkageTypes Linkage,
    llvm::Constant *Initializer,
    const llvm::Twine &Name,
    llvm::GlobalVariable *InsertBefore,
    llvm::GlobalVariable::ThreadLocalMode ThreadLocalMode,
    std::optional<unsigned> AddressSpace,
    bool isExternallyInitialized)
{
    llvm::GlobalVariable *GV = new llvm::GlobalVariable(
        M,
        Ty,
        isConstant,
        Linkage,
        Initializer,
        Name,
        InsertBefore,
        ThreadLocalMode,
        AddressSpace,
        isExternallyInitialized);

    GV->setAlignment(getAlign(Ty));

    return GV;
}

llvm::Function *CodeGenModule::createFunction(
    const FnType *fnType,
    llvm::GlobalValue::LinkageTypes linkage,
    const std::string &name)
{
    std::vector<llvm::Type *> paramTypes = getParamTypes(fnType);
    llvm::Type *originalRetType = getLLVMType(fnType->retType_.get());
    auto fnParams = abi_->getFunctionParams(originalRetType, paramTypes);

    std::vector<llvm::Type *> flatParams;
    for (const auto &p : fnParams.paramTypes)
    {
        flatParams.insert(flatParams.end(), p.begin(), p.end());
    }

    // Create the function
    llvm::FunctionType *ft = llvm::FunctionType::get(
        fnParams.retType, flatParams, /* isVarArg */ false);
    llvm::Function *fn =
        llvm::Function::Create(ft, linkage, name, module_.get());

    // Add metadata
    if (fnParams.structReturnInMemory)
    {
        fn->addParamAttr(0, llvm::Attribute::DeadOnUnwind);
        fn->addParamAttr(0, llvm::Attribute::NoAlias);
        fn->addParamAttr(0, llvm::Attribute::Writable);
        fn->addParamAttr(
            0,
            llvm::Attribute::getWithStructRetType(*context_, originalRetType));
    }

    unsigned argPtr = fnParams.structReturnInMemory ? 1 : 0;
    for (size_t i = argPtr; i < fnParams.paramTypes.size(); i++)
    {
        size_t astIndex = i - fnParams.structReturnInMemory;
        std::string paramName = fnType->getParamName(astIndex);
        llvm::Type *paramType = getLLVMType(fnType->getParamType(astIndex));

        // Label the function arguments
        for (size_t j = 0; j < fnParams.paramTypes[i].size(); j++)
        {
            // Pointer types are all NoUndef
            if (fnParams.paramTypes[i][j]->isPointerTy())
            {
                fn->addParamAttr(argPtr, llvm::Attribute::NoUndef);
            }

            if (paramType->isStructTy())
            {
                if (fnParams.paramTypes[i].size() > 1)
                {
                    // This is a struct that expands into 2 arguments
                }
                else if (fnParams.paramTypes[i][j]->isPointerTy())
                {
                    // This is a struct that is passed by memory
                    if (abi_->useByVal())
                    {
                        fn->addParamAttr(
                            argPtr,
                            llvm::Attribute::getWithByValType(
                                *context_, paramType));
                        fn->addParamAttr(
                            argPtr,
                            llvm::Attribute::getWithAlignment(
                                *context_,
                                getAlign(
                                    llvm::PointerType::get(paramType, 0))));
                    }
                    fn->getArg(argPtr)->setName(paramName);
                }
            }
            else
            {
                // This is a normal argument
                fn->getArg(argPtr)->setName(paramName);
            }

            argPtr++;
        }
    }

    return fn;
}

llvm::Align CodeGenModule::getAlign(llvm::Type *type) const
{
    return abi_->getTypeAlign(type);
}

llvm::Function *CodeGenModule::getCurrentFunction() const
{
    return builder_->GetInsertBlock()->getParent();
}

std::string CodeGenModule::getLocalStaticName(const std::string &name) const
{
    // If already seen before, return "name.1" (local statics can only be
    // defined once, if it's defined again, it's in another scope, i.e. fresh)
    static std::unordered_map<std::string, int> counter;
    counter[name]++;
    if (counter[name] > 1)
    {
        return getCurrentFunction()->getName().str() + "." + name + "." +
               std::to_string(counter[name]);
    }

    return getCurrentFunction()->getName().str() + "." + name;
}

llvm::Type *CodeGenModule::getLLVMType(const BaseNode *node)
{
    return getLLVMType(nodeMap_.at(node).get());
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
        std::vector<llvm::Type *> paramTypes = getParamTypes(fnType);
        return abi_->getFunctionType(
            getLLVMType(fnType->retType_.get()), paramTypes);
    }
    else if (auto arrType = dynamic_cast<const ArrayType *>(type))
    {
        // Must place ABOVE PtrType as it inherits PtrType
        return llvm::ArrayType::get(
            getLLVMType(arrType->type_.get()), arrType->size_);
    }
    else if (auto ptrType = dynamic_cast<const PtrType *>(type))
    {
        // Opaque pointers do not contain the type they point to
        return llvm::PointerType::get(*context_, 0);
    }
    else if (auto structType = dynamic_cast<const StructType *>(type))
    {
        // Structs are not defined here
        std::string name = "struct." + structType->name_;
        auto it = std::find(
            structIDs_[name].begin(),
            structIDs_[name].end(),
            structType->getID());

        size_t index = it - structIDs_[name].begin();
        if (index != 0)
        {
            name += "." + std::to_string(index);
        }

        if (auto *ty = llvm::StructType::getTypeByName(*context_, name))
        {
            return ty;
        }

        // Opaque structs
        return llvm::StructType::create(*context_, name);
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
    unsigned size = abi_->getTypeSize(ty);

    switch (ty)
    {
    case Types::VOID:
        return llvm::Type::getVoidTy(*context_);
    case Types::BOOL:
    case Types::CHAR:
    case Types::UNSIGNED_CHAR:
    case Types::SHORT:
    case Types::UNSIGNED_SHORT:
    case Types::INT:
    case Types::UNSIGNED_INT:
    case Types::LONG:
    case Types::UNSIGNED_LONG:
    case Types::LONG_LONG:
    case Types::UNSIGNED_LONG_LONG:
        return llvm::Type::getIntNTy(*context_, size);
    case Types::FLOAT:
    case Types::DOUBLE:
    case Types::LONG_DOUBLE:
        if (size == 32)
        {
            return llvm::Type::getFloatTy(*context_);
        }
        else if (size == 64)
        {
            return llvm::Type::getDoubleTy(*context_);
        }
        else if (size == 80)
        {
            return llvm::Type::getX86_FP80Ty(*context_);
        }
        else if (size == 128)
        {
            return llvm::Type::getFP128Ty(*context_);
        }
    }

    throw std::runtime_error("Unknown type");
}

std::vector<llvm::Type *> CodeGenModule::getParamTypes(const FnType *fnType)
{
    std::vector<llvm::Type *> paramTypes;
    for (const auto &p : fnType->params_->types_)
    {
        paramTypes.push_back(getLLVMType(p.second.get()));
    }

    return paramTypes;
}

llvm::Type *CodeGenModule::getPointerElementType(const BaseNode *node)
{
    if (auto *ty = dynamic_cast<const ArrayType *>(nodeMap_[node].get()))
    {
        return getLLVMType(ty->type_.get());
    }
    else if (auto *ty = dynamic_cast<const PtrType *>(nodeMap_[node].get()))
    {
        return getLLVMType(ty->type_.get());
    }

    throw std::runtime_error("Expected PtrType");
}

llvm::Value *CodeGenModule::visitAsLValue(const Expr &node)
{
    valueCategory_ = ValueCategory::LVALUE;
    node.accept(*this);

    // If no allocaInst is found, a temporary is assigned
    if (currentValue_->getType()->isPointerTy())
    {
        return currentValue_;
    }

    llvm::AllocaInst *allocaInst =
        createAlignedAlloca(getLLVMType(nodeMap_[&node].get()));
    builder_->CreateStore(currentValue_, allocaInst);
    return allocaInst;
}

llvm::Value *CodeGenModule::visitAsRValue(const Expr &node)
{
    // Everything can be decayed into an RValue
    valueCategory_ = ValueCategory::RVALUE;
    node.accept(*this);
    return currentValue_;
}

llvm::Value *CodeGenModule::visitAsCastedRValue(
    const Expr &node,
    const BaseType *expectedType)
{
    auto *initialType = nodeMap_[&node].get();

    if (getLLVMType(initialType)->isArrayTy())
    {
        // Array to pointer decay (array types cannot be passed)
        valueCategory_ = ValueCategory::LVALUE;
    }
    else
    {
        valueCategory_ = ValueCategory::RVALUE;
    }

    node.accept(*this);
    return runCast(currentValue_, initialType, expectedType);
}

llvm::Function *CodeGenModule::visitAsFnDesignator(const Expr &node)
{
    valueCategory_ = ValueCategory::FN_DESIGNATOR;
    node.accept(*this);
    return static_cast<llvm::Function *>(currentValue_);
}

llvm::Constant *
CodeGenModule::visitAsConstant(const Expr &node, const BaseType *expectedType)
{
    valueCategory_ = ValueCategory::RVALUE;
    ScopeGuard sg2(currentExpectedType_, expectedType);
    node.accept(*this);
    return static_cast<llvm::Constant *>(currentValue_);
}

llvm::Value *CodeGenModule::visitAsStore(
    const Expr &node,
    llvm::Value *storeVal,
    const BaseType *expectedType)
{
    ScopeGuard sg(currentStore_, static_cast<llvm::Value *>(storeVal));
    ScopeGuard sg2(currentExpectedType_, expectedType);
    llvm::Type *ty = getLLVMType(expectedType);

    if (ty->isStructTy())
    {
        // Not guaranteed to be a struct of the correct type
        llvm::Value *val = visitAsRValue(node);

        // Case: struct x = { 1, 2 }; (no need to do anything)
        if (!val)
        {
            return currentStore_;
        }

        // Case: struct x f(); (returns struct x in memory)
        if (val->getType()->isPointerTy())
        {
            llvm::Value *gep = builder_->CreateGEP(
                ty,
                currentStore_,
                {builder_->getInt32(0), builder_->getInt32(0)});
            builder_->CreateMemCpy(
                gep,
                getAlign(ty),
                val,
                getAlign(val->getType()),
                module_->getDataLayout().getTypeAllocSize(ty));
        }
        // Case: struct x f(); (returns { i64, i8 } or i8)
        else if (val->getType() != ty)
        {
            llvm::Value *gep = builder_->CreateGEP(
                ty,
                currentStore_,
                {builder_->getInt32(0), builder_->getInt32(0)});
            auto *tempAlloca = createAlignedAlloca(val->getType());
            builder_->CreateStore(val, tempAlloca);
            builder_->CreateMemCpy(
                gep,
                getAlign(ty),
                tempAlloca,
                getAlign(val->getType()),
                module_->getDataLayout().getTypeAllocSize(ty));
        }

        return val;
    }
    else if (ty->isArrayTy())
    {
        llvm::Value *val = visitAsRValue(node);

        // Case: int x[3] = { 1, 2 }; (no need to do anything)
        if (!val)
        {
            return val;
        }
        // Case: int x[3] = f(); (returns a pointer to an array)
        else if (val->getType()->isPointerTy())
        {
            builder_->CreateMemCpy(
                currentStore_,
                getAlign(ty),
                val,
                getAlign(val->getType()),
                module_->getDataLayout().getTypeAllocSize(ty));
        }

        return val;
    }

    llvm::Value *val = visitAsCastedRValue(node, currentExpectedType_);
    builder_->CreateStore(val, currentStore_);

    return val;
}

void CodeGenModule::symbolTablePush(std::string id, Symbol symbol)
{
    symbolTable_.back()[id] = symbol;
}

Symbol CodeGenModule::symbolTableLookup(std::string id) const
{
    for (auto it = symbolTable_.rbegin(); it != symbolTable_.rend(); ++it)
    {
        auto search = it->find(id);
        if (search != it->end())
        {
            return search->second;
        }
    }

    return std::monostate{};
}

void CodeGenModule::pushScope()
{
    symbolTable_.push_back(std::unordered_map<std::string, Symbol>());
}

void CodeGenModule::popScope()
{
    symbolTable_.pop_back();
}

llvm::Value *CodeGenModule::isNotZero(llvm::Value *val)
{
    llvm::Type *type = val->getType();
    if (type->isFloatingPointTy())
    {
        return builder_->CreateFCmpONE(val, llvm::ConstantFP::get(type, 0));
    }
    else if (type->isIntegerTy(1))
    {
        // Already a boolean, no need to compare
        return val;
    }
    else if (type->isIntegerTy())
    {
        return builder_->CreateICmpNE(val, llvm::ConstantInt::get(type, 0));
    }
    else if (type->isPointerTy())
    {
        return builder_->CreateICmpNE(
            val,
            llvm::ConstantPointerNull::get(
                llvm::cast<llvm::PointerType>(type)));
    }
    throw std::runtime_error("Unknown type");
}

Types CodeGenModule::getArithmeticConversionType(
    const BaseType *lhs,
    const BaseType *rhs)
{
    auto lhsBasic = dynamic_cast<const BasicType *>(lhs);
    auto rhsBasic = dynamic_cast<const BasicType *>(rhs);

    if (!lhsBasic || !rhsBasic)
    {
        return Types::VOID;
    }

    return TypeChecker::runUsualArithmeticConversions(
        lhsBasic->type_, rhsBasic->type_);
}

llvm::Value *CodeGenModule::runConversions(
    const BaseType *lhs,
    const BaseType *rhs,
    llvm::Value *lhsVal)
{
    // lhsVal = 0 (nullptr), rhs = PtrType
    if (auto val = llvm::dyn_cast<llvm::Constant>(lhsVal))
    {
        if (val->isZeroValue() && rhs->isArrayOrPtrTy())
        {
            return runCast(lhsVal, lhs, rhs);
        }
    }

    return runUsualArithmeticConversions(lhs, rhs, lhsVal);
}

llvm::Value *CodeGenModule::runUsualArithmeticConversions(
    const BaseType *lhs,
    const BaseType *rhs,
    llvm::Value *lhsVal)
{
    Types t = getArithmeticConversionType(lhs, rhs);

    if (t == Types::VOID)
    {
        return lhsVal;
    }

    auto *expectedType = new BasicType(t);

    return runCast(lhsVal, lhs, expectedType);
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

llvm::Value *CodeGenModule::runCast(
    llvm::Value *val,
    const BaseType *initialType,
    const BaseType *expectedType)
{
    if (!val)
    {
        return val;
    }

    // Will try to do anything to make the types match
    auto valType = val->getType();
    auto type = getLLVMType(expectedType);

    if (valType == type)
    {
        return val;
    }
    if (valType->isPointerTy() && type->isIntegerTy())
    {
        return builder_->CreatePtrToInt(val, type);
    }
    if (valType->isIntegerTy() && type->isPointerTy())
    {
        return builder_->CreateIntToPtr(val, type);
    }
    if (valType->isPointerTy() && type->isPointerTy())
    {
        return builder_->CreateBitCast(val, type);
    }

    // Should be basic types now
    auto *initialBasic = dynamic_cast<const BasicType *>(initialType);
    auto *expectedBasic = dynamic_cast<const BasicType *>(expectedType);

    if (!initialBasic || !expectedBasic)
    {
        return val;
    }

    bool initialSigned = initialBasic->isSigned();
    bool expectedSigned = expectedBasic->isSigned();

    if (valType->isFloatingPointTy() && type->isIntegerTy())
    {
        return (expectedSigned) ? builder_->CreateFPToSI(val, type)
                                : builder_->CreateFPToUI(val, type);
    }
    if (valType->isIntegerTy() && type->isFloatingPointTy())
    {
        return (initialSigned) ? builder_->CreateSIToFP(val, type)
                               : builder_->CreateUIToFP(val, type);
    }

    // int -> long (sext)
    // int -> unsigned long (sext, C99 6.3.1.3)
    // unsigned int -> long (zext)
    // unsigned int -> unsigned long (zext)
    bool isSigned = initialSigned;

    if (valType->isIntegerTy(1) && type->isIntegerTy())
    {
        // Special case for booleans
        return builder_->CreateZExt(val, type);
    }
    if (valType->isIntegerTy() && type->isIntegerTy())
    {
        return builder_->CreateIntCast(val, type, isSigned);
    }
    if (valType->isFloatingPointTy() && type->isFloatingPointTy())
    {
        return builder_->CreateFPCast(val, type);
    }

    return val;
}

} // namespace CodeGen
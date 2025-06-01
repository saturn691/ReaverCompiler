#include "CodeGen/AArch64ABI.hpp"

// Refer to
// https://github.com/ARM-software/abi-aa/blob/main/aapcs64/aapcs64.rst
// Unlike x86_64, LLVM handles register allocation of AArch64 mostly in the
// backend. Therefore the rules are just rough.

namespace CodeGen
{
/******************************************************************************
 *                          Private methods                                   *
 *****************************************************************************/

namespace
{

/// Returns 0 for error
size_t getAggregateNumElements(llvm::Type *type)
{
    if (type->isStructTy())
    {
        return type->getStructNumElements();
    }
    else if (type->isArrayTy())
    {
        return type->getArrayNumElements();
    }
    else
    {
        return 0;
    }
}

std::vector<llvm::Type *> flattenHFA(llvm::Type *t)
{
    // The test for homogeneity is applied after data layout is completed
    std::vector<llvm::Type *> flatElements;

    if (auto *structType = llvm::dyn_cast<llvm::StructType>(t))
    {
        for (size_t i = 0; i < structType->getNumElements(); i++)
        {
            auto v = flattenHFA(structType->getElementType(i));
            flatElements.insert(flatElements.end(), v.begin(), v.end());
        }
    }
    else if (auto *arrayType = llvm::dyn_cast<llvm::ArrayType>(t))
    {
        for (size_t i = 0; i < arrayType->getNumElements(); i++)
        {
            auto v = flattenHFA(arrayType->getElementType());
            flatElements.insert(flatElements.end(), v.begin(), v.end());
        }
    }
    else
    {
        flatElements.push_back(t);
    }

    return flatElements;
}

/// Homogeneous Floating-point Aggregates
bool isHFA(llvm::Type *type)
{
    if (!type->isAggregateType())
    {
        return false;
    }

    std::vector<llvm::Type *> flatElements = flattenHFA(type);
    if (flatElements.size() < 1 || flatElements.size() > 4)
    {
        return false;
    }

    return flatElements[0]->isFloatingPointTy() && std::equal(
                                                       flatElements.begin() + 1,
                                                       flatElements.end(),
                                                       flatElements.begin());
}

/// Homogeneous Short-Vector Aggregates
constexpr bool isHVA(llvm::Type *type)
{
    // e.g. float32x4_t
    return false;
}

} // namespace

/******************************************************************************
 *                          Public methods                                    *
 *****************************************************************************/

AArch64ABI::AArch64ABI(llvm::Module &module) : module_(module)
{
}

ABI::FunctionParamsInfo AArch64ABI::getFunctionParams(
    llvm::Type *retType,
    std::vector<llvm::Type *> &paramTypes) const
{
    // ABI 6.8.2: Parameter passing rules
    std::vector<std::vector<llvm::Type *>> actualParamTypes;
    bool structReturnInMemory = false;

    // ABI 6.9: Result return
    if (retType->isAggregateType())
    {
        auto paramType = getParamType(retType);
        if (paramType[0]->isPointerTy())
        {
            actualParamTypes.push_back(paramType);
            structReturnInMemory = true;
            retType = llvm::Type::getVoidTy(module_.getContext());
        }
        else
        {
            // structs in LLVM are pointers. This is to pass by value.
            retType = paramType[0];
        }
    }

    // Stage A: Initialization (N/A, reg allocation handled in backend)
    // In fact, we can handle them one at a time, without previous context
    for (auto *paramType : paramTypes)
    {
        actualParamTypes.push_back(getParamType(paramType));
    }

    return {retType, actualParamTypes, structReturnInMemory};
}

llvm::FunctionType *AArch64ABI::getFunctionType(
    llvm::Type *retType,
    std::vector<llvm::Type *> &paramTypes) const
{
    auto actualParamTypes = getFunctionParams(retType, paramTypes);

    std::vector<llvm::Type *> flatParamTypes;
    for (const auto &paramTypes : actualParamTypes.paramTypes)
    {
        flatParamTypes.insert(
            flatParamTypes.end(), paramTypes.begin(), paramTypes.end());
    }

    return llvm::FunctionType::get(
        actualParamTypes.retType, flatParamTypes, false);
}

std::vector<llvm::Type *> AArch64ABI::getParamType(llvm::Type *type) const
{
    size_t typeSize = module_.getDataLayout().getTypeAllocSize(type);

    // Stage B: Pre-padding and extension of arguments
    if (!isHFA(type) && !isHVA(type) && typeSize > 16)
    {
        // B.4. Passed via memory
        return {llvm::PointerType::get(type, 0)};
    }

    if (type->isAggregateType())
    {
        // B.5. Align to nearest multiple of 8
        typeSize = llvm::alignTo(typeSize, 8);
    }

    // Stage C: Assignment of arguments to register and stack
    // A lot of this happens in the backend. We only need aggregate
    // handling.
    if (isHFA(type))
    {
        // Decompose the HFA
        auto v = flattenHFA(type);
        return {llvm::ArrayType::get(v[0], v.size())};
    }
    else if (type->isAggregateType())
    {
        // LLVM takes care of everything else in the backend
        // C.11. This could be { int, float } or { int, int } -> use GP regs
        size_t regsRequired = typeSize / 8;
        auto i64 = llvm::Type::getInt64Ty(module_.getContext());
        if (regsRequired == 1)
        {
            return {i64};
        }

        return {llvm::ArrayType::get(i64, regsRequired)};
    }

    return {type};
}

llvm::Align AArch64ABI::getTypeAlign(llvm::Type *type) const
{
    // Opaque structs have no size
    if (type->isStructTy() && type->getStructNumElements() == 0)
    {
        return llvm::Align(1);
    }

    // _Alignof returns minimum align (e.g. _Alignof(int[4]) = 4, but LLVM
    // defaults to giving 16, as int[4] >= 16 bytes)
    int align = module_.getDataLayout().getPrefTypeAlign(type).value();

    return llvm::Align(align);
}

unsigned AArch64ABI::getTypeSize(AST::Types ty) const
{
    using Types = AST::Types;

    switch (ty)
    {
    case Types::VOID:
        return 0;
    case Types::BOOL:
        return 1;
    case Types::CHAR:
    case Types::UNSIGNED_CHAR:
        return 8;
    case Types::SHORT:
    case Types::UNSIGNED_SHORT:
        return 16;
    case Types::INT:
    case Types::UNSIGNED_INT:
        return 32;
    case Types::LONG:
    case Types::UNSIGNED_LONG:
        // ABI 10.1.2: This depends on ILP32/LP64/LLP64.
        // We don't support Windows, so use LP64
    case Types::LONG_LONG:
    case Types::UNSIGNED_LONG_LONG:
        return 64;

    case Types::FLOAT:
        return 32;
    case Types::DOUBLE:
        return 64;
    case Types::LONG_DOUBLE:
        return 128;
    };

    throw std::runtime_error("Unknown type");
}

} // namespace CodeGen
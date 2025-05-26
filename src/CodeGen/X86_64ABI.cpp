#include "CodeGen/X86_64ABI.hpp"

// Refer to
// https://lafibre.info/images/doc/202402_intel_application_binary_interface.pdf

namespace CodeGen
{
/******************************************************************************
 *                          Public methods                                    *
 *****************************************************************************/

X86_64ABI::X86_64ABI(llvm::Module &module) : module_(module)
{
}

ABI::FunctionParamsInfo X86_64ABI::getFunctionParams(
    llvm:: Type *retType,
    std::vector<llvm::Type *> &paramTypes) const
{
    // ABI 3.2.3: Parameter Passing
    constexpr unsigned INT_REGS = 6;
    constexpr unsigned SSE_REGS = 8;
    unsigned intRegs = 0;
    unsigned sseRegs = 0;
    std::vector<std::vector<llvm::Type *>> actualParamTypes;
    bool structReturnInMemory = false;

    // Return type can be passed by memory
    if (!retType->isVoidTy() &&
        getArgClassification(retType)[0].cls == ArgClass::MEMORY)
    {
        actualParamTypes.push_back({llvm::PointerType::get(retType, 0)});
        intRegs++;
        retType = llvm::Type::getVoidTy(module_.getContext());
        structReturnInMemory = true;
    }
    else
    {
        auto types = getParamType(retType);
        if (types.size() == 1)
        {
            // struct { i32 } -> i32
            retType = types[0];
        }
        else
        {
            // struct.pair -> { i32, i32 }
            retType = llvm::StructType::get(module_.getContext(), types);
        }
    }

    for (auto *paramType : paramTypes)
    {
        auto tys = getParamType(paramType);
        std::vector<llvm::Type *> currentParamTypes;

        if (tys.size() > 1)
        {
            // See if it can fit
            unsigned sseRegsRequired = 0;
            unsigned intRegsRequired = 0;
            for (size_t i = 0; i < tys.size(); i++)
            {
                if (tys[i]->isFloatingPointTy())
                {
                    sseRegsRequired++;
                }
                else
                {
                    intRegsRequired++;
                }
            }
            bool canFit = (intRegs + intRegsRequired <= INT_REGS) &&
                          (sseRegs + sseRegsRequired <= SSE_REGS);

            if (canFit)
            {
                currentParamTypes.insert(
                    currentParamTypes.end(), tys.begin(), tys.end());
                intRegs += intRegsRequired;
                sseRegs += sseRegsRequired;
            }
            else
            {
                currentParamTypes.push_back(llvm::PointerType::get(paramType, 0));
                intRegs++;
            }
        }
        else
        {
            if (tys[0]->isFloatingPointTy())
            {
                currentParamTypes.push_back(tys[0]);
                sseRegs++;
            }
            else
            {
                currentParamTypes.push_back(tys[0]);
                intRegs++;
            }
        }

        actualParamTypes.push_back(currentParamTypes);
    }

    return {retType, actualParamTypes, structReturnInMemory};
}

llvm::FunctionType *X86_64ABI::getFunctionType(
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

std::vector<llvm::Type *> X86_64ABI::getParamType(llvm::Type *type) const
{
    std::vector<llvm::Type *> actualParamTypes;

    if (!type->isAggregateType() || type->isX86_FP80Ty())
    {
        actualParamTypes.push_back(type);
    }
    else
    {
        auto argClasses = getArgClassification(type);

        // Struct, with 1 or 2 eightbytes
        for (size_t i = 0; i < argClasses.size(); i++)
        {
            switch (argClasses[i].cls)
            {
            case ArgClass::INTEGER:
                actualParamTypes.push_back(llvm::Type::getIntNTy(
                    module_.getContext(),
                    llvm::alignTo(argClasses[i].size, argClasses[i].align) *
                        8));
                break;
            case ArgClass::SSE:
                // Either { float, float } or { double }
                if (argClasses[i].multiple)
                {
                    actualParamTypes.push_back(llvm::VectorType::get(
                        llvm::Type::getFloatTy(module_.getContext()),
                        2,
                        false));
                }
                else
                {
                    actualParamTypes.push_back(
                        llvm::Type::getDoubleTy(module_.getContext()));
                }
                break;
            case ArgClass::MEMORY:
                // argClasses.size() must be 1
                actualParamTypes.push_back(llvm::PointerType::get(type, 0));
                break;
            default:
                throw std::runtime_error("Unknown argument class");
            }
        }
    }

    return actualParamTypes;
}

llvm::Align X86_64ABI::getTypeAlign(llvm::Type *type) const
{
    // Opaque structs have no size
    if (type->isStructTy() && type->getStructNumElements() == 0)
    {
        return llvm::Align(1);
    }

    // _Alignof returns minimum align (e.g. _Alignof(int[4]) = 4, but LLVM
    // defaults to giving 16, as int[4] >= 16 bytes)
    int align = module_.getDataLayout().getPrefTypeAlign(type).value();
    int size = module_.getDataLayout().getTypeAllocSize(type);

    // See ABI 3.1.2: Aggregates and Unions

    if (type->isArrayTy() && size >= 16)
    {
        return llvm::Align(std::max(16, align));
    }

    return llvm::Align(align);
}

unsigned X86_64ABI::getTypeSize(AST::Types ty) const
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
    case Types::LONG_LONG:
    case Types::UNSIGNED_LONG_LONG:
        return 64;
    case Types::FLOAT:
        return 32;
    case Types::DOUBLE:
        return 64;
    case Types::LONG_DOUBLE:
        return 80;
    };

    throw std::runtime_error("Unknown type");
}

/******************************************************************************
 *                          Private methods                                   *
 *****************************************************************************/

X86_64ABI::ArgClasses X86_64ABI::getArgClassification(llvm::Type *type) const
{
    // ABI 3.2.3: Parameter Passing
    // Complex long double not supported yet
    const auto &layout = module_.getDataLayout();
    unsigned sizeBytes = layout.getTypeAllocSize(type);
    unsigned align = getTypeAlign(type).value();
    auto returnMemory = ArgClasses{{ArgClass::MEMORY, sizeBytes, align, false}};

    if (type->isIntegerTy() || type->isPointerTy())
    {
        return {{ArgClass::INTEGER, sizeBytes, align, false}};
    }
    else if (type->isX86_FP80Ty())
    {
        // Alignment = 12 or 16 bytes
        unsigned trailingBytes = layout.getTypeAllocSize(type) - 8;
        return {
            {ArgClass::X87, 8, 8, false},
            {ArgClass::X87UP, trailingBytes, trailingBytes, false}};
    }
    else if (type->isFloatingPointTy())
    {
        return {{ArgClass::SSE, sizeBytes, false}};
    }
    else if (type->isAggregateType())
    {
        unsigned numEightbytes = (sizeBytes + 7) / 8;
        std::vector<ArgClassInfo> eightbytes(
            numEightbytes, {ArgClass::NO_CLASS, 0, 0, false});

        if (sizeBytes > 64)
        {
            return returnMemory;
        }
        size_t numElements = (type->isStructTy()) ? type->getStructNumElements()
                                                  : type->getArrayNumElements();

        // 2. If a C++ object is non trivial... (ignore, C++ specific)

        for (size_t i = 0; i < numElements; i++)
        {
            llvm::Type *elemType;
            size_t elemOffset;
            size_t elemSize;

            if (type->isStructTy())
            {
                llvm::StructType *structType =
                    llvm::cast<llvm::StructType>(type);
                const auto *structLayout = layout.getStructLayout(structType);

                elemType = type->getStructElementType(i);
                elemOffset = structLayout->getElementOffset(i);
                elemSize = layout.getTypeAllocSize(elemType);
            }
            else
            {
                llvm::ArrayType *arrayType = llvm::cast<llvm::ArrayType>(type);

                elemType = arrayType->getArrayElementType();
                elemOffset = (uint64_t)i * layout.getTypeAllocSize(elemType);
                elemSize = layout.getTypeAllocSize(elemType);
            }

            ArgClasses elemClass = getArgClassification(elemType);

            for (size_t j = 0; j < elemClass.size(); j++)
            {
                ArgClassInfo cls = elemClass[j];
                size_t classSize = cls.size;
                size_t startByte = elemOffset + (j * 8); // inclusive
                size_t endByte = startByte + classSize;  // exclusive
                size_t nextBoundary = (startByte / 8 * 8) + 8;
                size_t eightByte = startByte / 8;

                // Could span over 1 or 2 eightbytes (eightbyte <= 8 bytes)
                cls.size = std::min(endByte, nextBoundary) - startByte;
                eightbytes[eightByte] =
                    mergeClassifications(eightbytes[eightByte], cls);
                if (classSize != cls.size)
                {
                    cls.size = classSize - cls.size;
                    eightbytes[eightByte + 1] =
                        mergeClassifications(eightbytes[eightByte + 1], cls);
                }
            }
        }

        // 5. Then a post merger cleanup is done
        if (std::any_of(
                eightbytes.begin(),
                eightbytes.end(),
                [](auto &e) { return e.cls == ArgClass::MEMORY; }))
        {
            return returnMemory;
        }
        // If X87UP is present, X87 must be directly before it
        for (size_t i = 0; i < numEightbytes; i++)
        {
            if (eightbytes[i].cls == ArgClass::X87UP)
            {
                if (i == 0 || eightbytes[i - 1].cls != ArgClass::X87)
                {
                    return returnMemory;
                }
            }
        }

        if (sizeBytes > 16)
        {
            // If SSE is the first eightbyte, all other eightbytes must be SSEUP
            if (eightbytes[0].cls == ArgClass::SSE)
            {
                for (size_t i = 1; i < numEightbytes; i++)
                {
                    if (eightbytes[i].cls != ArgClass::SSEUP)
                    {
                        return returnMemory;
                    }
                }
            }
            else
            {
                return returnMemory;
            }
        }

        // If SSEUP is not preceded by SSE/SSEUP, it is converted to SSE
        for (size_t i = 1; i < numEightbytes; i++)
        {
            if (eightbytes[i].cls == ArgClass::SSEUP &&
                eightbytes[i - 1].cls != ArgClass::SSE &&
                eightbytes[i - 1].cls != ArgClass::SSEUP)
            {
                eightbytes[i].cls = ArgClass::SSE;
            }
        }

        // Remove trailing NO_CLASS
        while (!eightbytes.empty() &&
               eightbytes.back().cls == ArgClass::NO_CLASS)
        {
            eightbytes.pop_back();
        }

        return eightbytes;
    }

    // Undefined
    return {};
}

X86_64ABI::ArgClassInfo X86_64ABI::mergeClassifications(
    X86_64ABI::ArgClassInfo lhs,
    X86_64ABI::ArgClassInfo rhs) const
{
    unsigned size = std::min(8u, lhs.size + rhs.size);
    unsigned align = std::max(lhs.align, rhs.align);
    if (lhs.cls == rhs.cls)
    {
        return {lhs.cls, size, align, true};
    }
    if (lhs.cls == ArgClass::NO_CLASS)
    {
        return {rhs.cls, size, align, false};
    }
    if (rhs.cls == ArgClass::NO_CLASS)
    {
        return {lhs.cls, size, align, false};
    }
    if (lhs.cls == ArgClass::MEMORY || rhs.cls == ArgClass::MEMORY)
    {
        return {ArgClass::MEMORY, size, align, true};
    }
    if (lhs.cls == ArgClass::INTEGER || rhs.cls == ArgClass::INTEGER)
    {
        return {ArgClass::INTEGER, size, align, true};
    }
    if (lhs.cls == ArgClass::X87 || rhs.cls == ArgClass::X87 ||
        lhs.cls == ArgClass::X87UP || rhs.cls == ArgClass::X87UP ||
        lhs.cls == ArgClass::COMPLEX_X87 || rhs.cls == ArgClass::COMPLEX_X87)
    {
        return {ArgClass::MEMORY, size, align, true};
    }

    return {ArgClass::SSE, size, align, true};
}

} // namespace CodeGen
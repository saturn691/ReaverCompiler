#include "CodeGen/TypeChecker.hpp"
#include "AST/Decl.hpp"
#include "AST/Expr.hpp"
#include "AST/Stmt.hpp"
#include "AST/Type.hpp"

#include "CodeGen/ScopeGuard.hpp"

#include <algorithm>
#include <iostream>
#include <limits>

namespace CodeGen
{
TypeChecker::TypeChecker(std::ostream &os) : os_(os)
{
    typeContext_.push_back({});
}

/******************************************************************************
 *                          Declarations                                      *
 *****************************************************************************/

void TypeChecker::visit(const ArrayDecl &node)
{
    // Type check the size
    node.size_->accept(*this);
    assertIsIntegerTy(typeMap_[node.size_.get()].get());

    // Attempt to get a size (otherwise, VLA)
    auto size = node.size_->eval();

    // Does not instantiate a type, rather passes information down
    Ptr<BaseType> oldType = currentType_->clone();
    if (size)
    {
        currentType_ =
            std::make_unique<ArrayType>(currentType_->clone(), *size);
    }
    else
    {
        currentType_ = std::make_unique<ArrayType>(currentType_->clone(), 0);
    }
    node.decl_->accept(*this);
    typeMap_[&node] = typeMap_[node.decl_.get()]->clone();
    currentType_ = std::move(oldType);
}

void TypeChecker::visit(const BasicTypeDecl &node)
{
    typeMap_[&node] = std::make_unique<BasicType>(node.type_);
}

void TypeChecker::visit(const CompoundTypeDecl &node)
{
    // Default to int if no type is found (e.g. signed)
    currentType_ = std::make_unique<BasicType>(Types::INT);

    // First pass: Find base type
    for (const auto &variant : node.nodes_)
    {
        std::visit(
            [this](const auto &decl)
            {
                if (!dynamic_cast<const TypeModifier *>(decl.get()))
                {
                    decl->accept(*this);
                    currentType_ = typeMap_[decl.get()]->clone();
                }
            },
            variant);
    }

    // Second pass: Apply modifiers
    for (const auto &variant : node.nodes_)
    {
        std::visit(
            [this](const auto &decl)
            {
                if (dynamic_cast<const TypeModifier *>(decl.get()))
                {
                    decl->accept(*this);
                }
            },
            variant);
    }

    typeMap_[&node] = currentType_->clone();
}

void TypeChecker::visit(const DeclNode &node)
{
    // Pass type information down
    node.type_->accept(*this);
    Ptr<BaseType> type = typeMap_[node.type_.get()]->clone();
    typeMap_[&node] = type->clone();

    // Type check the initializer list
    if (node.initDeclList_)
    {
        currentType_ = type->clone();
        node.initDeclList_->accept(*this);
    }
}

void TypeChecker::visit(const DefinedTypeDecl &node)
{
    typeMap_[&node] = lookupType(node.getID());
}

void TypeChecker::visit(const Enum &node)
{
    if (node.members_)
    {
        // Only does type checking
        node.members_->accept(*this);

        EnumConsts enumConsts;
        int lastSeenVal = -1;
        for (const auto &member : node.members_->nodes_)
        {
            auto enumMember = std::get<0>(member).get();

            int val = (enumMember->expr_) ? *enumMember->expr_->eval()
                                          : lastSeenVal + 1;
            enumConsts.push_back({enumMember->getID(), val});
            lastSeenVal = val;
        }

        typeMap_[&node] = std::make_unique<EnumType>(node.name_, enumConsts);
        insertType(node.getID(), typeMap_[&node]->clone());
    }
    else
    {
        // Grab the definition, if it exists
        if (auto t = lookupType(node.getID()))
        {
            typeMap_[&node] = std::move(t);
        }
        else
        {
            typeMap_[&node] =
                std::make_unique<EnumType>(node.name_, EnumConsts());
        }
    }
}

void TypeChecker::visit(const EnumMember &node)
{
    // Only checks the type of the expression
    if (node.expr_)
    {
        node.expr_->accept(*this);
        assertIsIntegerTy(typeMap_[node.expr_.get()].get());
    }

    insertType(node.getID(), std::make_unique<BasicType>(Types::INT));
}

void TypeChecker::visit(const EnumMemberList &node)
{
    for (const auto &member : node.nodes_)
    {
        std::visit(
            [this](const auto &member) { member->accept(*this); }, member);
    }
}

void TypeChecker::visit(const FnDecl &node)
{
    Ptr<BaseType> retType = currentType_->clone();
    node.params_->accept(*this);
    Ptr<ParamType> params =
        dynamic_cast<const ParamType *>(typeMap_[node.params_.get()].get())
            ->cloneAsDerived();

    auto ty = std::make_unique<FnType>(std::move(params), std::move(retType));
    typeMap_[&node] = ty->clone();

    insertType(node.getID(), std::move(ty));
}

void TypeChecker::visit(const FnDef &node)
{
    currentFunction_ = &node;
    node.retType_->accept(*this);
    currentType_ = typeMap_[node.retType_.get()]->clone();

    // Add enough context to the type map, including the parameters
    node.decl_->accept(*this);
    typeMap_[&node] = typeMap_[node.decl_.get()]->clone();

    // Parameters gets its own scope
    pushScope();

    // Add the parameters to the context
    auto *fnType =
        dynamic_cast<const FnType *>(typeMap_[node.decl_.get()].get());
    for (const auto &param : fnType->params_->types_)
    {
        insertType(param.first, param.second->clone());
    }

    // Now run the type check (on the body)
    node.body_->accept(*this);
    popScope();
}

void TypeChecker::visit(const InitDecl &node)
{
    // This must insert the type into the context
    // Why? Because of the parser (e.g. `int a();` or `int *b;`)
    {
        ScopeGuard<bool> guard(fromDecl_, true);
        node.decl_->accept(*this);
    }
    Ptr<BaseType> expectedType = typeMap_[node.decl_.get()]->clone();
    insertType(node.getID(), expectedType->clone());

    if (node.expr_)
    {
        node.expr_->accept(*this);

        // Check the type of the expression
        auto *actual = typeMap_[node.expr_.get()].get();
        checkType(actual, expectedType.get());
    }

    typeMap_[&node] = std::move(expectedType);
}

void TypeChecker::visit(const InitDeclList &node)
{
    for (const auto &initDecl : node.nodes_)
    {
        std::visit(
            [this](const auto &initDecl) { initDecl->accept(*this); },
            initDecl);
    }
}

void TypeChecker::visit(const ParamDecl &node)
{
    // Pass type information down
    node.type_->accept(*this);
    currentType_ = typeMap_[node.type_.get()]->clone();

    // Don't insert into context yet, just collect types (we scan node.decl_ for
    // compound types)
    if (node.decl_)
    {
        ScopeGuard<bool> guard(fromDecl_, true);
        node.decl_->accept(*this);
        typeMap_[&node] = typeMap_[node.decl_.get()]->clone();
    }
    else
    {
        typeMap_[&node] = currentType_->clone();
    }
}

void TypeChecker::visit(const ParamList &node)
{
    Params types;
    for (const auto &param : node.nodes_)
    {
        std::visit(
            [this, &types](const auto &param)
            {
                param->accept(*this);
                types.push_back(
                    {param->getID(), typeMap_[param.get()]->clone()});
            },
            param);
    }

    // Parameter List can be the single keyword void (no parameters)
    if (types.size() == 1 && types[0].first == "" &&
        dynamic_cast<const BasicType *>(types[0].second.get())->type_ ==
            Types::VOID)
    {
        types.clear();
    }

    typeMap_[&node] = std::make_unique<ParamType>(std::move(types));
}

void TypeChecker::visit(const PtrDecl &node)
{
    // Does not instantiate a type, rather passes information down
    Ptr<BaseType> oldType = currentType_->clone();
    node.ptr_->accept(*this);
    node.decl_->accept(*this);
    typeMap_[&node] = typeMap_[node.decl_.get()]->clone();
    currentType_ = std::move(oldType);
}

void TypeChecker::visit(const PtrNode &node)
{
    // Changes the type
    currentType_ = std::make_unique<PtrType>(currentType_->clone());
}

void TypeChecker::visit(const Struct &node)
{
    if (node.members_)
    {
        // If it's a definition, we need to add to the context
        // Will "return" a ParamType
        node.members_->accept(*this);
        Ptr<ParamType> members =
            dynamic_cast<const ParamType *>(typeMap_[node.members_.get()].get())
                ->cloneAsDerived();

        typeMap_[&node] = std::make_unique<StructType>(
            StructType::Type::STRUCT, node.name_, std::move(members));
        insertType(node.getID(), typeMap_[&node]->clone());
    }
    else
    {
        // Grab the definition, if it exists
        if (auto t = lookupType(node.getID()))
        {
            typeMap_[&node] = t->clone();
        }
        else
        {
            typeMap_[&node] = std::make_unique<StructType>(
                StructType::Type::STRUCT, node.name_);
        }
    }
}

void TypeChecker::visit(const StructDecl &node)
{
    typeMap_[&node] = currentType_->clone();
}

void TypeChecker::visit(const StructDeclList &node)
{
    Params types;

    for (const auto &decl : node.nodes_)
    {
        std::visit(
            [this, &types](const auto &decl)
            {
                // "Returns" any type
                decl->accept(*this);
                types.push_back({decl->getID(), typeMap_[decl.get()]->clone()});
            },
            decl);
    }

    typeMap_[&node] = std::make_unique<ParamType>(std::move(types));
}

void TypeChecker::visit(const StructMember &node)
{
    node.type_->accept(*this);
    currentType_ = typeMap_[node.type_.get()]->clone();
    node.declList_->accept(*this);
    typeMap_[&node] = typeMap_[node.declList_.get()]->clone();
}

void TypeChecker::visit(const StructMemberList &node)
{
    Params types;

    for (const auto &member : node.nodes_)
    {
        std::visit(
            [this, &types](const auto &member)
            {
                // "Returns" a ParamType
                member->accept(*this);
                auto *paramType = dynamic_cast<const ParamType *>(
                    typeMap_[member.get()].get());
                for (const auto &type : paramType->types_)
                {
                    types.push_back({type.first, type.second->clone()});
                }
            },
            member);
    }

    typeMap_[&node] = std::make_unique<ParamType>(std::move(types));
}

void TypeChecker::visit(const TranslationUnit &node)
{
    for (const auto &decl : node.nodes_)
    {
        std::visit([this](const auto &decl) { decl->accept(*this); }, decl);
    }
}

void TypeChecker::visit(const Typedef &node)
{
    // Actually REALLY simple to implement, this can be treated EXACTLY like a
    // normal declaration
    node.type_->accept(*this);
    typeMap_[&node] = typeMap_[node.type_.get()]->clone();
}

void TypeChecker::visit(const TypeModifier &node)
{
    using Complex = TypeModifier::Complex;
    using Length = TypeModifier::Length;
    using Signedness = TypeModifier::Signedness;
    using StorageClass = TypeModifier::StorageClass;

    auto visitComplex = [this](Complex c)
    {
        // MUST be a BasicType
        auto *basicType =
            dynamic_cast<const BasicType *>(this->currentType_.get());
        if (!basicType)
        {
            throw std::runtime_error("Expected basic type");
        }

        Types t;
        switch (c)
        {
        case Complex::COMPLEX:
            switch (basicType->type_)
            {
            case Types::FLOAT:
                t = Types::FLOAT_COMPLEX;
                break;
            case Types::DOUBLE:
                t = Types::DOUBLE_COMPLEX;
                break;
            case Types::LONG_DOUBLE:
                t = Types::LONG_DOUBLE_COMPLEX;
                break;
            default:
                throw std::runtime_error("Invalid type for complex modifier");
            }
            break;
        case Complex::IMAGINARY:
            switch (basicType->type_)
            {
            case Types::FLOAT:
                t = Types::FLOAT_IMAGINARY;
                break;
            case Types::DOUBLE:
                t = Types::DOUBLE_IMAGINARY;
                break;
            case Types::LONG_DOUBLE:
                t = Types::LONG_DOUBLE_IMAGINARY;
                break;
            default:
                throw std::runtime_error("Invalid type for imaginary modifier");
            }
            break;
        }

        this->currentType_ = std::make_unique<BasicType>(t);
    };
    auto visitCVRQualifier = [this](CVRQualifier cvr)
    {
        switch (cvr)
        {
        case CVRQualifier::CONST:
            this->currentType_->cvrQualifier_ = CVRQualifier::CONST;
            break;
        case CVRQualifier::VOLATILE:
            this->currentType_->cvrQualifier_ = CVRQualifier::VOLATILE;
            break;
        case CVRQualifier::RESTRICT:
            this->currentType_->cvrQualifier_ = CVRQualifier::RESTRICT;
            break;
        }
    };
    auto visitFunctionSpecifier = [this](FunctionSpecifier fs)
    {
        switch (fs)
        {
        case FunctionSpecifier::INLINE:
            this->currentType_->functionSpecifier_ = FunctionSpecifier::INLINE;
            break;
        }
    };
    auto visitLength = [this](Length l)
    {
        // MUST be a BasicType
        auto *basicType =
            dynamic_cast<const BasicType *>(this->currentType_.get());
        if (!basicType)
        {
            throw std::runtime_error("Expected basic type");
        }

        Types t;
        switch (basicType->type_)
        {
        case Types::INT:
            t = Types::LONG;
            break;
        case Types::UNSIGNED_INT:
            t = Types::UNSIGNED_LONG;
            break;
        case Types::LONG:
            t = Types::LONG_LONG;
            break;
        case Types::UNSIGNED_LONG:
            t = Types::UNSIGNED_LONG_LONG;
            break;
        default:
            throw std::runtime_error("Invalid type for length modifier");
        }

        this->currentType_ = std::make_unique<BasicType>(t);
    };
    auto visitSignedness = [this](Signedness s)
    {
        // MUST be a BasicType
        auto *basicType =
            dynamic_cast<const BasicType *>(this->currentType_.get());
        if (!basicType)
        {
            throw std::runtime_error("Expected basic type");
        }

        switch (s)
        {
        case Signedness::SIGNED:
            break; // This is default behaviour, no need to do anything
        case Signedness::UNSIGNED:
            Types ty;
            switch (basicType->type_)
            {
            case Types::CHAR:
                ty = Types::UNSIGNED_CHAR;
                break;
            case Types::SHORT:
                ty = Types::UNSIGNED_SHORT;
                break;
            case Types::INT:
                ty = Types::UNSIGNED_INT;
                break;
            case Types::LONG:
                ty = Types::UNSIGNED_LONG;
                break;
            case Types::LONG_LONG:
                ty = Types::UNSIGNED_LONG_LONG;
                break;
            default:
                throw std::runtime_error(
                    "Invalid type for signedness modifier");
            }
            this->currentType_ = std::make_unique<BasicType>(ty);
            break;
        }
    };
    auto visitStorageClass = [this](StorageClass sc)
    {
        switch (sc)
        {
        case StorageClass::AUTO:
        case StorageClass::REGISTER:
            this->currentType_->linkage_ = Linkage::NONE;
            this->currentType_->storageDuration_ = StorageDuration::AUTO;
            break;
        case StorageClass::STATIC:
            this->currentType_->linkage_ = Linkage::INTERNAL;
            this->currentType_->storageDuration_ = StorageDuration::STATIC;
            break;
        case StorageClass::EXTERN:
            this->currentType_->linkage_ = Linkage::EXTERNAL;
            this->currentType_->storageDuration_ = StorageDuration::STATIC;
            break;
        }
    };

    std::visit(
        overloads{
            visitComplex,
            visitCVRQualifier,
            visitFunctionSpecifier,
            visitLength,
            visitSignedness,
            visitStorageClass},
        node.modifier_);
}

/******************************************************************************
 *                          Expressions                                       *
 *****************************************************************************/

void TypeChecker::visit(const ArrayAccess &node)
{
    // Check the type of the array
    node.arr_->accept(*this);
    auto *arrayType = typeMap_[node.arr_.get()].get();
    if (auto *t = dynamic_cast<const ArrayType *>(arrayType))
    {
        typeMap_[&node] = t->type_->clone();
    }
    else if (auto *t = dynamic_cast<const PtrType *>(arrayType))
    {
        typeMap_[&node] = t->type_->clone();
    }
    else
    {
        os_ << "Error: Expected array or pointer type" << std::endl;
    }

    // Check the type of the index
    node.index_->accept(*this);
    auto *indexType = typeMap_[node.index_.get()].get();
    assertIsIntegerTy(indexType);
}

void TypeChecker::visit(const Assignment &node)
{
    // Subject to integer promotion rules
    node.lhs_->accept(*this);
    node.rhs_->accept(*this);

    auto *lhs = typeMap_[node.lhs_.get()].get();
    auto *rhs = typeMap_[node.rhs_.get()].get();

    // TODO: Do this properly
    typeMap_[&node] = lhs->clone();
}

void TypeChecker::visit(const ArgExprList &node)
{
    Params types;
    for (const auto &arg : node.nodes_)
    {
        std::visit(
            [this, &types](const auto &arg)
            {
                arg->accept(*this);
                // Ignore the name of the parameter
                types.push_back({"", typeMap_[arg.get()]->clone()});
            },
            arg);
    }
    typeMap_[&node] = std::make_unique<ParamType>(std::move(types));
}

void TypeChecker::visit(const BinaryOp &node)
{
    using Op = BinaryOp::Op;

    // Subject to integer promotion rules
    node.lhs_->accept(*this);
    node.rhs_->accept(*this);

    auto *lhs = typeMap_[node.lhs_.get()].get();
    auto *rhs = typeMap_[node.rhs_.get()].get();

    switch (node.op_)
    {
    case Op::EQ:
    case Op::NE:
    case Op::LT:
    case Op::GT:
    case Op::LE:
    case Op::GE:
        // Result is always a bool
        typeMap_[&node] = std::make_unique<BasicType>(Types::BOOL);
        break;
    default:
        // Pointer arithmetic
        auto *lhsPtr = dynamic_cast<const PtrType *>(lhs);
        auto *rhsPtr = dynamic_cast<const PtrType *>(rhs);
        if (lhsPtr)
        {
            if (rhsPtr)
            {
                // Pointer subtraction
                typeMap_[&node] = std::make_unique<BasicType>(Types::INT);
            }
            else
            {
                // Pointer arithmetic
                typeMap_[&node] = lhsPtr->clone();
            }
        }
        else if (rhsPtr)
        {
            // Pointer arithmetic
            typeMap_[&node] = rhsPtr->clone();
        }
        else
        {
            // Must be BasicType
            auto *lhsBasic = dynamic_cast<const BasicType *>(lhs);
            auto *rhsBasic = dynamic_cast<const BasicType *>(rhs);
            if (!lhsBasic || !rhsBasic)
            {
                os_ << "Error: Expected basic type" << std::endl;
                return;
            }
            typeMap_[&node] =
                std::make_unique<BasicType>(runUsualArithmeticConversions(
                    lhsBasic->type_, rhsBasic->type_));
        }
    }
}

void TypeChecker::visit(const Cast &node)
{
    // Check the type of the expression
    node.expr_->accept(*this);

    // Check the type of the cast
    node.type_->accept(*this);
    auto *castType = typeMap_[node.type_.get()].get();
    auto *exprType = typeMap_[node.expr_.get()].get();
    checkType(exprType, castType);

    typeMap_[&node] = castType->clone();
}

void TypeChecker::visit(const Constant &node)
{
    // C99 6.4.4 Constants
    Types t;
    std::string value = node.value_;

    if (value.find('.') != std::string::npos ||
        value.find('e') != std::string::npos ||
        value.find('E') != std::string::npos ||
        value.find('p') != std::string::npos ||
        value.find('P') != std::string::npos)
    {
        // Floating point constant
        if (value.back() == 'f' || value.back() == 'F')
        {
            t = Types::FLOAT;
        }
        else if (value.back() == 'l' || value.back() == 'L')
        {
            t = Types::LONG_DOUBLE;
        }
        else
        {
            t = Types::DOUBLE;
        }
    }
    else if (value.find("'") != std::string::npos)
    {
        // Character constant
        if (value.size() == 3 || (value[1] == '\\' && value.size() == 4))
        {
            t = Types::CHAR;
        }
        else
        {
            t = Types::INT;
        }
    }
    else
    {
        // Integer constant
        auto x = value.find_first_not_of("0123456789x");
        std::string suffix = (x == std::string::npos) ? "" : value.substr(x);
        bool isUnsigned = suffix.find('u') != std::string::npos ||
                          suffix.find('U') != std::string::npos;
        bool isLong = suffix.find('l') != std::string::npos ||
                      suffix.find('L') != std::string::npos;
        bool isLongLong = suffix.find("ll") != std::string::npos ||
                          suffix.find("LL") != std::string::npos;

        // Helper function to determine the type of the integer constant
        auto getTypeForValue = [](const std::string &value, bool isUnsigned)
        {
            if (isUnsigned)
            {
                unsigned long long val = std::stoull(value);
                if (val <= std::numeric_limits<unsigned int>::max())
                {
                    return Types::UNSIGNED_INT;
                }
                else if (val <= std::numeric_limits<unsigned long>::max())
                {
                    return Types::UNSIGNED_LONG;
                }
                else
                {
                    return Types::UNSIGNED_LONG_LONG;
                }
            }
            else
            {
                long long val = std::stoll(value);
                if (val <= std::numeric_limits<int>::max() &&
                    val >= std::numeric_limits<int>::min())
                {
                    return Types::INT;
                }
                else if (
                    val <= std::numeric_limits<long>::max() &&
                    val >= std::numeric_limits<long>::min())
                {
                    return Types::LONG;
                }
                else
                {
                    return Types::LONG_LONG;
                }
            }
        };

        if (isLongLong)
        {
            t = isUnsigned ? Types::UNSIGNED_LONG_LONG : Types::LONG_LONG;
        }
        else if (isLong)
        {
            t = isUnsigned
                    ? std::max(
                          Types::UNSIGNED_LONG, getTypeForValue(value, true))
                    : std::max(Types::LONG, getTypeForValue(value, false));
        }
        else
        {
            t = getTypeForValue(value, isUnsigned);
        }
    }

    typeMap_[&node] = std::make_unique<BasicType>(t);
}

void TypeChecker::visit(const FnCall &node)
{
    // Check the type of the function
    node.fn_->accept(*this);
    auto *fnType = dynamic_cast<const FnType *>(typeMap_[node.fn_.get()].get());
    if (!fnType)
    {
        os_ << "Error: Expected function type" << std::endl;
        throw std::runtime_error("Expected function type");
        return;
    }

    if (node.args_)
    {
        node.args_->accept(*this);

        // Check the type of the arguments
        auto *argType =
            dynamic_cast<const ParamType *>(typeMap_[node.args_.get()].get());
        if (!argType)
        {
            os_ << "Error: Expected parameter type" << std::endl;
            return;
        }

        // Check the number of arguments
        if (fnType->params_->size() != argType->size())
        {
            os_ << "Error: Expected " << fnType->params_->size() << " arguments"
                << std::endl;
            return;
        }

        // Check the types of the arguments
        for (size_t i = 0; i < fnType->params_->size(); ++i)
        {
            checkType(argType->at(i), fnType->params_->at(i));
        }

        // All get casted to the declared types
        typeMap_[node.args_.get()] = fnType->params_->clone();
    }

    typeMap_[&node] = fnType->retType_->clone();
}

void TypeChecker::visit(const Identifier &node)
{
    // Problem is that Identifier is both an Expr and a Decl
    if (fromDecl_)
    {
        typeMap_[&node] = currentType_->clone();
    }
    else
    {
        typeMap_[&node] = lookupType(node.getID());
    }
}

void TypeChecker::visit(const Paren &node)
{
    node.expr_->accept(*this);
    typeMap_[&node] = typeMap_[node.expr_.get()]->clone();
}

void TypeChecker::visit(const SizeOf &node)
{
    if (node.expr_)
    {
        node.expr_->accept(*this);
    }
    else
    {
        node.type_->accept(*this);
    }
    typeMap_[&node] = std::make_unique<BasicType>(Types::UNSIGNED_INT);
}

void TypeChecker::visit(const StringLiteral &node)
{
    // + 1 for null terminator
    typeMap_[&node] = std::make_unique<ArrayType>(
        std::make_unique<BasicType>(Types::CHAR), node.value_.size() + 1);
}

void TypeChecker::visit(const StructAccess &node)
{
    // Check the type of the struct
    node.expr_->accept(*this);
    auto *structType = typeMap_[node.expr_.get()].get();
    if (auto *t = dynamic_cast<const StructType *>(structType))
    {
        typeMap_[&node] = t->getMemberType(node.member_)->clone();
    }
    else
    {
        os_ << "Error: Expected struct type" << std::endl;
    }
}

void TypeChecker::visit(const StructPtrAccess &node)
{
    // Check the type of the struct
    node.expr_->accept(*this);
    auto *structType = typeMap_[node.expr_.get()].get();
    if (auto *t = dynamic_cast<const PtrType *>(structType))
    {
        if (auto *s = dynamic_cast<const StructType *>(t->type_.get()))
        {
            typeMap_[&node] = s->getMemberType(node.member_)->clone();
        }
        else
        {
            os_ << "Error: Expected struct type" << std::endl;
        }
    }
    else
    {
        os_ << "Error: Expected pointer type" << std::endl;
    }
}

void TypeChecker::visit(const TernaryOp &node)
{
    // Condition can be any type, but will be converted to a boolean
    node.cond_->accept(*this);
    node.lhs_->accept(*this);
    node.rhs_->accept(*this);

    auto *thenExpr = typeMap_[node.lhs_.get()].get();
    auto *elseExpr = typeMap_[node.rhs_.get()].get();

    // Check the type of the expressions
    auto *thenExprBasic = dynamic_cast<const BasicType *>(thenExpr);
    auto *elseExprBasic = dynamic_cast<const BasicType *>(elseExpr);

    if (thenExprBasic && elseExprBasic)
    {
        typeMap_[&node] =
            std::make_unique<BasicType>(runUsualArithmeticConversions(
                thenExprBasic->type_, elseExprBasic->type_));
    }
    else
    {
        // TODO Not 100% correct. See 6.5.15
        typeMap_[&node] = typeMap_[node.lhs_.get()]->clone();
    }
}

void TypeChecker::visit(const UnaryOp &node)
{
    node.expr_->accept(*this);

    auto *actual = typeMap_[node.expr_.get()].get();
    switch (node.op_)
    {
    case UnaryOp::Op::ADDR:
        typeMap_[&node] = std::make_unique<PtrType>(actual->clone());
        break;
    case UnaryOp::Op::DEREF:
        if (auto *ptr = dynamic_cast<const PtrType *>(actual))
        {
            typeMap_[&node] = ptr->type_->clone();
        }
        else
        {
            os_ << "Error: Expected pointer type" << std::endl;
        }
        break;
    case UnaryOp::Op::PLUS:
    case UnaryOp::Op::MINUS:
        if (!dynamic_cast<const BasicType *>(actual))
        {
            os_ << "Error: Expected arithmetic type" << std::endl;
        }
        typeMap_[&node] = actual->clone();
        break;
    case UnaryOp::Op::NOT:
    case UnaryOp::Op::LNOT:
        assertIsIntegerTy(actual);
        typeMap_[&node] = actual->clone();
        break;
    case UnaryOp::Op::POST_DEC:
    case UnaryOp::Op::POST_INC:
    case UnaryOp::Op::PRE_DEC:
    case UnaryOp::Op::PRE_INC:
        if (!(*actual <= BasicType(Types::INT)))
        {
            os_ << "Error: Expected implicit integer type" << std::endl;
        }
        typeMap_[&node] = actual->clone();
        break;
    }
}

/******************************************************************************
 *                          Statements                                        *
 *****************************************************************************/

void TypeChecker::visit(const BlockItemList &node)
{
    for (const auto &item : node.nodes_)
    {
        std::visit([this](const auto &item) { item->accept(*this); }, item);
    }
}

void TypeChecker::visit(const Break &node)
{
    // Nothing to do
}

void TypeChecker::visit(const Case &node)
{
    if (node.expr_)
    {
        node.expr_->accept(*this);

        // Case expression must be an integer
        auto *actual = typeMap_[node.expr_.get()].get();
        if (!(*actual == BasicType(Types::INT)))
        {
            os_ << "Error: Expected integer type (Case)" << std::endl;
            return;
        }
    }

    node.body_->accept(*this);
}

void TypeChecker::visit(const CompoundStmt &node)
{
    if (node.nodes_)
    {
        pushScope();
        node.nodes_->accept(*this);
        popScope();
    }
}

void TypeChecker::visit(const Continue &node)
{
    // Nothing to do
}

void TypeChecker::visit(const DoWhile &node)
{
    node.body_->accept(*this);
    node.cond_->accept(*this);
}

void TypeChecker::visit(const ExprStmt &node)
{
    if (node.expr_)
    {
        node.expr_->accept(*this);
        typeMap_[&node] = typeMap_[node.expr_.get()]->clone();
    }
}

void TypeChecker::visit(const For &node)
{
    std::visit([this](const auto &init) { init->accept(*this); }, node.init_);
    node.cond_->accept(*this);
    if (node.expr_)
    {
        node.expr_->accept(*this);
    }
    node.body_->accept(*this);
}

void TypeChecker::visit(const IfElse &node)
{
    node.cond_->accept(*this);

    // Condition can be any type, but will be converted to a boolean
    auto *actual = typeMap_[node.cond_.get()].get();

    node.thenStmt_->accept(*this);
    if (node.elseStmt_)
    {
        node.elseStmt_->accept(*this);
    }
}

void TypeChecker::visit(const Return &node)
{
    node.expr_->accept(*this);

    // Return type must match FnDef return type
    auto *actual = typeMap_[node.expr_.get()].get();

    // Cast expected to FnType
    auto *t = typeMap_[currentFunction_].get();
    auto *fnType = dynamic_cast<const FnType *>(t);
    if (!fnType)
    {
        os_ << "Error: Expected function type" << std::endl;
        return;
    }

    checkType(actual, fnType->retType_.get());

    // Will be casted to the return type of the function
    typeMap_[&node] = fnType->retType_->clone();
}

void TypeChecker::visit(const Switch &node)
{
    node.expr_->accept(*this);

    // Expression must be an integer
    auto *actual = typeMap_[node.expr_.get()].get();
    if (!(*actual == BasicType(Types::INT)))
    {
        os_ << "Error: Expected integer type (Switch)" << std::endl;
        return;
    }

    node.body_->accept(*this);
}

void TypeChecker::visit(const While &node)
{
    node.cond_->accept(*this);

    // Condition can be any type, but will be converted to a boolean
    auto *actual = typeMap_[node.cond_.get()].get();

    node.body_->accept(*this);
}

/******************************************************************************
 *                          Static methods                                    *
 *****************************************************************************/

Types TypeChecker::runIntegerPromotions(Types types)
{
    // 6.3.1.1: "If an int can represent all values of the original type, the
    // value is converted to an int; otherwise, it is converted to an unsigned
    // int. These are called the integer promotions."

    if (types == Types::VOID)
    {
        throw std::runtime_error("Error: Cannot promote void type");
    }
    else if (types <= Types::INT)
    {
        return Types::INT;
    }
    else if (types <= Types::UNSIGNED_INT)
    {
        return Types::UNSIGNED_INT;
    }

    return types;
}

Types TypeChecker::runUsualArithmeticConversions(Types lhs, Types rhs)
{
    // 6.3.1.8 Usual arithmetic conversions
    // "First, if the corresponding real type of either operand is long double,
    // the other operand is converted, without change of type domain, to a type
    // whose corresponding real type is long double."
    if (lhs == Types::LONG_DOUBLE || rhs == Types::LONG_DOUBLE)
    {
        return Types::LONG_DOUBLE;
    }
    // "Otherwise, if the corresponding real type of either operand is double,
    // the other operand is converted, without change of type domain, to a type
    // whose corresponding real type is double."
    if (lhs == Types::DOUBLE || rhs == Types::DOUBLE)
    {
        return Types::DOUBLE;
    }
    // "Otherwise, if the corresponding real type of either operand is float,
    // the other operand is converted, without change of type domain, to a type
    // whose corresponding real type is float."
    if (lhs == Types::FLOAT || rhs == Types::FLOAT)
    {
        return Types::FLOAT;
    }

    // "Otherwise, the integer promotions are performed on both operands. Then
    // the following rules are applied to the promoted operands:"
    lhs = runIntegerPromotions(lhs);
    rhs = runIntegerPromotions(rhs);

    // A lot of yap to say we want the larger type
    return (lhs > rhs) ? lhs : rhs;
}

/******************************************************************************
 *                          Private methods                                   *
 *****************************************************************************/

bool TypeChecker::checkType(const BaseType *actual, const BaseType *expected)
{
    if (!(*actual <= *expected))
    {
        os_ << "Error: Type mismatch" << std::endl;
        return false;
    }

    return true;
}

bool TypeChecker::assertIsIntegerTy(const BaseType *type)
{
    if (auto *basicType = dynamic_cast<const BasicType *>(type))
    {
        std::vector<Types> allowed = {
            Types::BOOL,
            Types::CHAR,
            Types::UNSIGNED_CHAR,
            Types::INT,
            Types::SHORT,
            Types::UNSIGNED_SHORT,
            Types::UNSIGNED_INT,
            Types::LONG,
            Types::UNSIGNED_LONG,
            Types::LONG_LONG,
            Types::UNSIGNED_LONG_LONG,
        };
        if (std::find(allowed.begin(), allowed.end(), basicType->type_) !=
            allowed.end())
        {
            return true;
        }
    }

    os_ << "Error: Expected integer type" << std::endl;
    return false;
}

void TypeChecker::pushScope()
{
    typeContext_.push_back({});
}

void TypeChecker::popScope()
{
    typeContext_.pop_back();
}

Ptr<BaseType> TypeChecker::lookupType(const std::string &name) const
{
    for (auto it = typeContext_.rbegin(); it != typeContext_.rend(); ++it)
    {
        if (it->find(name) != it->end())
        {
            return it->at(name)->clone();
        }
    }

    return nullptr;
}

void TypeChecker::insertType(const std::string &name, Ptr<BaseType> type)
{
    typeContext_.back()[name] = std::move(type);
}

} // namespace CodeGen
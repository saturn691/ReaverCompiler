#include "CodeGen/TypeChecker.hpp"
#include "AST/Decl.hpp"
#include "AST/Expr.hpp"
#include "AST/Stmt.hpp"
#include "AST/Type.hpp"

#include <algorithm>
#include <iostream>

namespace CodeGen
{
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

void TypeChecker::visit(const DeclNode &node)
{
    // Pass type information down
    Ptr<BaseType> type = node.type_->clone();
    typeMap_[&node] = type->clone();

    // Type check the initializer list
    currentType_ = type->clone();
    node.initDeclList_->accept(*this);
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
    typeContext_[node.getID()] = std::move(ty);
}

void TypeChecker::visit(const FnDef &node)
{
    currentFunction_ = node.decl_->getID();
    currentType_ = node.retType_->clone();

    // Add enough context to the type map
    node.decl_->accept(*this);
    typeMap_[&node] = typeMap_[node.decl_.get()]->clone();

    // Now run the type check (on the body)
    node.body_->accept(*this);
}

void TypeChecker::visit(const InitDecl &node)
{
    // This must insert the type into the context
    // Why? Because of the parser (e.g. `int a();` or `int *b;`)
    node.decl_->accept(*this);
    Ptr<BaseType> expectedType = typeMap_[node.decl_.get()]->clone();

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
    currentType_ = node.type_->clone();
    node.decl_->accept(*this);
    typeMap_[&node] = typeMap_[node.decl_.get()]->clone();
}

void TypeChecker::visit(const ParamList &node)
{
    std::vector<Ptr<BaseType>> types;
    for (const auto &param : node.nodes_)
    {
        std::visit(
            [this, &types](const auto &param)
            {
                param->accept(*this);
                types.push_back(typeMap_[param.get()]->clone());
            },
            param);
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

void TypeChecker::visit(const TranslationUnit &node)
{
    for (const auto &decl : node.nodes_)
    {
        std::visit([this](const auto &decl) { decl->accept(*this); }, decl);
    }
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
    std::vector<Ptr<BaseType>> types;
    for (const auto &arg : node.nodes_)
    {
        std::visit(
            [this, &types](const auto &arg)
            {
                arg->accept(*this);
                types.push_back(typeMap_[arg.get()]->clone());
            },
            arg);
    }
    typeMap_[&node] = std::make_unique<ParamType>(std::move(types));
}

void TypeChecker::visit(const BinaryOp &node)
{
    // Subject to integer promotion rules
    node.lhs_->accept(*this);
    node.rhs_->accept(*this);

    auto *lhs = typeMap_[node.lhs_.get()].get();
    auto *rhs = typeMap_[node.rhs_.get()].get();

    // TODO: Do this properly
    typeMap_[&node] = lhs->clone();
}

void TypeChecker::visit(const Constant &node)
{
    // Here we can infer the type of the constant
    Types t;
    std::string value = node.value_;

    // Floating point constant
    if (value.find('.') != std::string::npos ||
        value.find('e') != std::string::npos ||
        value.find('E') != std::string::npos ||
        value.find('p') != std::string::npos ||
        value.find('P') != std::string::npos)
    {
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
        // TODO wchar_t
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
        // TODO this is not exactly correct
        if (value.back() == 'u' || value.back() == 'U')
        {
            t = Types::UNSIGNED_INT;
        }
        else if (value.back() == 'l' || value.back() == 'L')
        {
            t = Types::LONG;
        }
        else
        {
            t = Types::INT;
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
    }

    typeMap_[&node] = fnType->retType_->clone();
}

void TypeChecker::visit(const Identifier &node)
{
    // Problem is that Identifier is both an Expr and a Decl
    if (typeContext_.find(node.getID()) == typeContext_.end())
    {
        // Case for Decl (implied, it won't be in the typeContext)
        typeMap_[&node] = currentType_->clone();
        typeContext_[node.getID()] = currentType_->clone();
    }
    else
    {
        // Case for Expr
        typeMap_[&node] = typeContext_.at(node.getID())->clone();
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
        if (!(*actual == BasicType(Types::INT) ||
              *actual == BasicType(Types::FLOAT) ||
              *actual == BasicType(Types::DOUBLE)))
        {
            os_ << "Error: Expected integer or floating point type"
                << std::endl;
        }
        typeMap_[&node] = actual->clone();
        break;
    case UnaryOp::Op::NOT:
    case UnaryOp::Op::LNOT:
        if (!(*actual == BasicType(Types::INT)))
        {
            os_ << "Error: Expected integer type" << std::endl;
        }
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

void TypeChecker::visit(const CompoundStmt &node)
{
    if (node.nodes_)
    {
        node.nodes_->accept(*this);
    }
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

    // Condition must be an integer
    auto *actual = typeMap_[node.cond_.get()].get();
    if (!(*actual == BasicType(Types::INT)))
    {
        os_ << "Error: Expected integer type" << std::endl;
        return;
    }

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
    auto *fnType =
        dynamic_cast<const FnType *>(typeContext_[currentFunction_].get());
    if (!fnType)
    {
        os_ << "Error: Expected function type for '" << currentFunction_ << "'"
            << std::endl;
        return;
    }

    checkType(actual, fnType->retType_.get());
}

void TypeChecker::visit(const While &node)
{
    node.cond_->accept(*this);

    // Condition can be any type, but will be converted to a boolean
    auto *actual = typeMap_[node.cond_.get()].get();

    node.body_->accept(*this);
}

/******************************************************************************
 *                          Types                                             *
 *****************************************************************************/

void TypeChecker::visit(const BasicType &node)
{
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
            Types::CHAR,
            Types::INT,
            Types::LONG,
            Types::UNSIGNED_CHAR,
            Types::UNSIGNED_INT,
            Types::UNSIGNED_LONG,
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

Types TypeChecker::runIntegerPromotion(Types types)
{
    // 6.3.1.1: "If an int can represent all values of the original type, the
    // value is converted to an int; otherwise, it is converted to an unsigned
    // int. These are called the integer promotions."

    if (types == Types::VOID)
    {
        os_ << "Error: Expected non-void type" << std::endl;
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

} // namespace CodeGen
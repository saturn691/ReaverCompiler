#include "CodeGen/TypeChecker.hpp"
#include "AST/Decl.hpp"
#include "AST/Expr.hpp"
#include "AST/Stmt.hpp"
#include "AST/Type.hpp"

#include <iostream>

namespace CodeGen
{
/******************************************************************************
 *                          Declarations                                      *
 *****************************************************************************/

void TypeChecker::visit(const DeclNode &node)
{
    // Pass type information down
    Ptr<BaseType> type = node.type_->clone();
    typeMap_[&node] = type->clone();

    // Type check the initializer list
    currentType_ = type.get();
    node.initDeclList_->accept(*this);
}

void TypeChecker::visit(const FnDecl &node)
{
    node.params_->accept(*this);
    Ptr<ParamType> params =
        dynamic_cast<const ParamType *>(typeMap_[node.params_.get()].get())
            ->cloneAsDerived();
    Ptr<BaseType> retType = currentType_->clone();

    auto ty = std::make_unique<FnType>(std::move(params), std::move(retType));
    typeMap_[&node] = ty->clone();
    typeContext_[node.getID()] = std::move(ty);
}

void TypeChecker::visit(const FnDef &node)
{
    currentFunction_ = node.decl_->getID();
    currentType_ = node.retType_.get();

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
    Ptr<BaseType> type = node.type_->clone();

    typeContext_[node.getID()] = type->clone();
    typeMap_[&node] = std::move(type);
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

    if (value.find('.') != std::string::npos)
    {
        t = Types::DOUBLE;
    }
    else
    {
        t = Types::INT;
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
            checkType(fnType->params_->at(i), argType->at(i));
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

    // Condition must be an integer
    auto *actual = typeMap_[node.cond_.get()].get();
    if (!(*actual == BasicType(Types::INT)))
    {
        os_ << "Error: Expected integer type" << std::endl;
        return;
    }

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
    if (!(*actual == *expected))
    {
        os_ << "Error: Type mismatch" << std::endl;
        return false;
    }

    return true;
}

} // namespace CodeGen
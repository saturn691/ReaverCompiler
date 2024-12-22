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
}

void TypeChecker::visit(const FnDecl &node)
{
    node.params_->accept(*this);
    typeMap_[&node] = typeMap_[node.params_.get()]->clone();
}

void TypeChecker::visit(const FnDef &node)
{
    currentFunction_ = node.decl_->getID();

    // Add enough context to the type map
    node.decl_->accept(*this);
    Ptr<ParamType> params =
        dynamic_cast<const ParamType *>(typeMap_[node.decl_.get()].get())
            ->cloneAsDerived();
    Ptr<BaseType> retType = node.retType_->clone();
    auto ty = std::make_unique<FnType>(std::move(params), std::move(retType));
    typeMap_[&node] = ty->clone();
    typeContext_[currentFunction_] = std::move(ty);

    // Now run the type check (on the body)
    node.body_->accept(*this);
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

void TypeChecker::visit(const Identifier &node)
{
    typeMap_[&node] = typeContext_.at(node.getID())->clone();
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
    node.nodes_->accept(*this);
}

void TypeChecker::visit(const Return &node)
{
    node.expr_->get()->accept(*this);

    // Return type must match FnDef return type
    auto *actual = typeMap_[node.expr_->get()].get();

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
#pragma once

#include <iostream>
#include <unordered_map>

#include "AST/Type.hpp"
#include "AST/Visitor.hpp"

using namespace AST;

namespace CodeGen
{
using TypeMap = std::unordered_map<const BaseNode *, Ptr<BaseType>>;
using TypeContext = std::unordered_map<std::string, Ptr<BaseType>>;

class TypeChecker : public Visitor
{
public:
    TypeChecker(std::ostream &os) : os_(os)
    {
    }

    // Declarations
    void visit(const DeclNode &node) override;
    void visit(const FnDecl &node) override;
    void visit(const FnDef &node) override;
    void visit(const InitDecl &node) override;
    void visit(const InitDeclList &node) override;
    void visit(const ParamDecl &node) override;
    void visit(const ParamList &node) override;
    void visit(const TranslationUnit &node) override;

    // Expressions
    void visit(const Assignment &node) override;
    void visit(const ArgExprList &node) override;
    void visit(const BinaryOp &node) override;
    void visit(const Constant &node) override;
    void visit(const FnCall &node) override;
    void visit(const Identifier &node) override;

    // Statements
    void visit(const BlockItemList &node) override;
    void visit(const CompoundStmt &node) override;
    void visit(const ExprStmt &node) override;
    void visit(const For &node) override;
    void visit(const IfElse &node) override;
    void visit(const Return &node) override;
    void visit(const While &node) override;

    // Types
    void visit(const BasicType &node) override;

    TypeMap &getTypeMap()
    {
        return typeMap_;
    }

private:
    TypeMap typeMap_;
    TypeContext typeContext_;
    std::ostream &os_;

    // Contextual information
    // Jumping around TUs
    std::string currentFunction_;
    // For DeclNode and FnDef
    const BaseType *currentType_;

    bool checkType(const BaseType *actual, const BaseType *expected);
};
} // namespace CodeGen
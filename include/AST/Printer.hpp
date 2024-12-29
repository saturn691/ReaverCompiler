#pragma once

#include <iostream>

#include "AST/Visitor.hpp"

namespace AST
{
class Printer : public Visitor
{
public:
    Printer(std::ostream &os) : os(os)
    {
    }

    // Declarations
    void visit(const ArrayDecl &node) override;
    void visit(const DeclNode &node) override;
    void visit(const FnDecl &node) override;
    void visit(const FnDef &node) override;
    void visit(const InitDecl &node) override;
    void visit(const InitDeclList &node) override;
    void visit(const ParamDecl &node) override;
    void visit(const ParamList &node) override;
    void visit(const PtrDecl &node) override;
    void visit(const PtrNode &node) override;
    void visit(const TranslationUnit &node) override;

    // Expressions
    void visit(const ArrayAccess &node) override;
    void visit(const Assignment &node) override;
    void visit(const ArgExprList &node) override;
    void visit(const BinaryOp &node) override;
    void visit(const Constant &node) override;
    void visit(const FnCall &node) override;
    void visit(const Identifier &node) override;
    void visit(const UnaryOp &node) override;

    // Statements
    void visit(const BlockItemList &node) override;
    void visit(const CompoundStmt &node) override;
    void visit(const IfElse &node) override;
    void visit(const For &node) override;
    void visit(const ExprStmt &node) override;
    void visit(const Return &node) override;
    void visit(const While &node) override;

    // Types
    void visit(const BasicType &node) override;

private:
    unsigned int indentLevel = 0;
    static const unsigned int indentWidth = 4;
    std::ostream &os;

    inline std::string getIndent() const
    {
        return std::string(indentLevel * indentWidth, ' ');
    }
};
} // namespace AST
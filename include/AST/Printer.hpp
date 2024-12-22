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
    void visit(const DeclNode &node) override;
    void visit(const FnDecl &node) override;
    void visit(const FnDef &node) override;
    void visit(const ParamDecl &node) override;
    void visit(const ParamList &node) override;
    void visit(const TranslationUnit &node) override;

    // Expressions
    void visit(const BinaryOp &node) override;
    void visit(const Constant &node) override;
    void visit(const Identifier &node) override;

    // Statements
    void visit(const BlockItemList &node) override;
    void visit(const CompoundStmt &node) override;
    void visit(const Return &node) override;

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
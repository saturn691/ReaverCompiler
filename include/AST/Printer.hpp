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
    void visit(const BasicTypeDecl &node) override;
    void visit(const DeclNode &node) override;
    void visit(const DefinedTypeDecl &node) override;
    void visit(const Enum &node) override;
    void visit(const EnumMember &node) override;
    void visit(const EnumMemberList &node) override;
    void visit(const FnDecl &node) override;
    void visit(const FnDef &node) override;
    void visit(const InitDecl &node) override;
    void visit(const InitDeclList &node) override;
    void visit(const ParamDecl &node) override;
    void visit(const ParamList &node) override;
    void visit(const PtrDecl &node) override;
    void visit(const PtrNode &node) override;
    void visit(const Struct &node) override;
    void visit(const StructDecl &node) override;
    void visit(const StructDeclList &node) override;
    void visit(const StructMember &node) override;
    void visit(const StructMemberList &node) override;
    void visit(const TranslationUnit &node) override;
    void visit(const Typedef &node) override;

    // Expressions
    void visit(const ArrayAccess &node) override;
    void visit(const Assignment &node) override;
    void visit(const ArgExprList &node) override;
    void visit(const BinaryOp &node) override;
    void visit(const Constant &node) override;
    void visit(const FnCall &node) override;
    void visit(const Identifier &node) override;
    void visit(const Paren &node) override;
    void visit(const SizeOf &node) override;
    void visit(const StringLiteral &node) override;
    void visit(const StructAccess &node) override;
    void visit(const StructPtrAccess &node) override;
    void visit(const TernaryOp &node) override;
    void visit(const UnaryOp &node) override;

    // Statements
    void visit(const BlockItemList &node) override;
    void visit(const Break &node) override;
    void visit(const Case &node) override;
    void visit(const CompoundStmt &node) override;
    void visit(const Continue &node) override;
    void visit(const DoWhile &node) override;
    void visit(const IfElse &node) override;
    void visit(const For &node) override;
    void visit(const ExprStmt &node) override;
    void visit(const Return &node) override;
    void visit(const Switch &node) override;
    void visit(const While &node) override;

private:
    unsigned int indentLevel_ = 0;
    static const unsigned int indentWidth = 4;
    std::ostream &os;

    inline std::string getIndent() const
    {
        return std::string(indentLevel_ * indentWidth, ' ');
    }
};
} // namespace AST
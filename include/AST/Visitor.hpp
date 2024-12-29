#pragma once

namespace AST
{
// Forward declarations
class ArrayAccess;
class ArrayDecl;
class Assignment;
class ArgExprList;
class BasicType;
class BinaryOp;
class BlockItemList;
class CompoundStmt;
class Constant;
class DeclNode;
class Expr;
class ExprStmt;
class FnCall;
class FnDecl;
class FnDef;
class For;
class Identifier;
class IfElse;
class InitDecl;
class InitDeclList;
class ParamDecl;
class ParamList;
class PtrDecl;
class PtrNode;
class Return;
class TranslationUnit;
class UnaryOp;
class While;

/**
 * Visitor design pattern for traversing the AST.
 */
class Visitor
{
public:
    virtual ~Visitor() = default;

    // Declarations
    virtual void visit(const ArrayDecl &node) = 0;
    virtual void visit(const DeclNode &node) = 0;
    virtual void visit(const FnDecl &node) = 0;
    virtual void visit(const FnDef &node) = 0;
    virtual void visit(const InitDecl &node) = 0;
    virtual void visit(const InitDeclList &node) = 0;
    virtual void visit(const ParamDecl &node) = 0;
    virtual void visit(const ParamList &node) = 0;
    virtual void visit(const PtrDecl &node) = 0;
    virtual void visit(const PtrNode &node) = 0;
    virtual void visit(const TranslationUnit &node) = 0;

    // Expressions
    virtual void visit(const ArrayAccess &node) = 0;
    virtual void visit(const Assignment &node) = 0;
    virtual void visit(const ArgExprList &node) = 0;
    virtual void visit(const BinaryOp &node) = 0;
    virtual void visit(const Constant &node) = 0;
    virtual void visit(const FnCall &node) = 0;
    virtual void visit(const Identifier &node) = 0;
    virtual void visit(const UnaryOp &node) = 0;

    // Statements
    virtual void visit(const BlockItemList &node) = 0;
    virtual void visit(const CompoundStmt &node) = 0;
    virtual void visit(const ExprStmt &node) = 0;
    virtual void visit(const For &node) = 0;
    virtual void visit(const IfElse &node) = 0;
    virtual void visit(const Return &node) = 0;
    virtual void visit(const While &node) = 0;

    // Types
    virtual void visit(const BasicType &node) = 0;
};
} // namespace AST
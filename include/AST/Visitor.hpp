#pragma once

namespace AST
{
// Forward declarations
class BasicType;
class BinaryOp;
class BlockItemList;
class CompoundStmt;
class Constant;
class DeclNode;
class FnDecl;
class FnDef;
class Identifier;
class ParamDecl;
class ParamList;
class Return;
class TranslationUnit;

/**
 * Visitor design pattern for traversing the AST.
 */
class Visitor
{
public:
    virtual ~Visitor() = default;

    // Declarations
    virtual void visit(const DeclNode &node) = 0;
    virtual void visit(const FnDecl &node) = 0;
    virtual void visit(const FnDef &node) = 0;
    virtual void visit(const ParamDecl &node) = 0;
    virtual void visit(const ParamList &node) = 0;
    virtual void visit(const TranslationUnit &node) = 0;

    // Expressions
    virtual void visit(const BinaryOp &node) = 0;
    virtual void visit(const Constant &node) = 0;
    virtual void visit(const Identifier &node) = 0;

    // Statements
    virtual void visit(const BlockItemList &node) = 0;
    virtual void visit(const CompoundStmt &node) = 0;
    virtual void visit(const Return &node) = 0;

    // Types
    virtual void visit(const BasicType &node) = 0;
};
} // namespace AST
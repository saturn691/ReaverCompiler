#pragma once

namespace AST
{
// Forward declarations
class ArrayAccess;
class ArrayDecl;
class Assignment;
class ArgExprList;
class BasicTypeDecl;
class BinaryOp;
class BlockItemList;
class Break;
class Case;
class CompoundStmt;
class Constant;
class Continue;
class DeclNode;
class DefinedTypeDecl;
class Enum;
class EnumMember;
class EnumMemberList;
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
class Paren;
class ParamDecl;
class ParamList;
class PtrDecl;
class PtrNode;
class SizeOf;
class StringLiteral;
class Struct;
class StructAccess;
class StructPtrAccess;
class StructDecl;
class StructDeclList;
class StructMember;
class StructMemberList;
class Switch;
class Return;
class TranslationUnit;
class Typedef;
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
    virtual void visit(const BasicTypeDecl &node) = 0;
    virtual void visit(const DeclNode &node) = 0;
    virtual void visit(const DefinedTypeDecl &node) = 0;
    virtual void visit(const Enum &node) = 0;
    virtual void visit(const EnumMember &node) = 0;
    virtual void visit(const EnumMemberList &node) = 0;
    virtual void visit(const FnDecl &node) = 0;
    virtual void visit(const FnDef &node) = 0;
    virtual void visit(const InitDecl &node) = 0;
    virtual void visit(const InitDeclList &node) = 0;
    virtual void visit(const ParamDecl &node) = 0;
    virtual void visit(const ParamList &node) = 0;
    virtual void visit(const PtrDecl &node) = 0;
    virtual void visit(const PtrNode &node) = 0;
    virtual void visit(const Struct &node) = 0;
    virtual void visit(const StructDecl &node) = 0;
    virtual void visit(const StructDeclList &node) = 0;
    virtual void visit(const StructMember &node) = 0;
    virtual void visit(const StructMemberList &node) = 0;
    virtual void visit(const TranslationUnit &node) = 0;
    virtual void visit(const Typedef &node) = 0;

    // Expressions
    virtual void visit(const ArrayAccess &node) = 0;
    virtual void visit(const Assignment &node) = 0;
    virtual void visit(const ArgExprList &node) = 0;
    virtual void visit(const BinaryOp &node) = 0;
    virtual void visit(const Constant &node) = 0;
    virtual void visit(const FnCall &node) = 0;
    virtual void visit(const Identifier &node) = 0;
    virtual void visit(const Paren &node) = 0;
    virtual void visit(const SizeOf &node) = 0;
    virtual void visit(const StringLiteral &node) = 0;
    virtual void visit(const StructAccess &node) = 0;
    virtual void visit(const StructPtrAccess &node) = 0;
    virtual void visit(const UnaryOp &node) = 0;

    // Statements
    virtual void visit(const BlockItemList &node) = 0;
    virtual void visit(const Break &node) = 0;
    virtual void visit(const Case &node) = 0;
    virtual void visit(const CompoundStmt &node) = 0;
    virtual void visit(const Continue &node) = 0;
    virtual void visit(const ExprStmt &node) = 0;
    virtual void visit(const For &node) = 0;
    virtual void visit(const IfElse &node) = 0;
    virtual void visit(const Return &node) = 0;
    virtual void visit(const Switch &node) = 0;
    virtual void visit(const While &node) = 0;
};
} // namespace AST
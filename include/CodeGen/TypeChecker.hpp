#pragma once

#include <iostream>
#include <unordered_map>

#include "AST/Node.hpp"
#include "AST/Type.hpp"
#include "AST/Visitor.hpp"

using namespace AST;

namespace CodeGen
{
using TypeMap = std::unordered_map<const BaseNode *, Ptr<BaseType>>;
using TypeContext = std::vector<std::unordered_map<std::string, Ptr<BaseType>>>;

class TypeChecker : public Visitor
{
public:
    TypeChecker(std::ostream &os);

    // Declarations
    void visit(const AbstractTypeDecl &node) override;
    void visit(const ArrayDecl &node) override;
    void visit(const BasicTypeDecl &node) override;
    void visit(const CompoundTypeDecl &node) override;
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
    void visit(const TypeModifier &node) override;

    // Expressions
    void visit(const ArrayAccess &node) override;
    void visit(const Assignment &node) override;
    void visit(const ArgExprList &node) override;
    void visit(const BinaryOp &node) override;
    void visit(const Cast &node) override;
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
    void visit(const ExprStmt &node) override;
    void visit(const For &node) override;
    void visit(const IfElse &node) override;
    void visit(const Return &node) override;
    void visit(const Switch &node) override;
    void visit(const While &node) override;

    TypeMap &getTypeMap()
    {
        return typeMap_;
    }

    static Types runIntegerPromotions(Types type);
    static Types runUsualArithmeticConversions(Types lhs, Types rhs);

private:
    TypeMap typeMap_;
    TypeContext typeContext_;
    std::ostream &os_;

    // Contextual information
    // Jumping around TUs
    const BaseNode *currentFunction_;
    // For DeclNode and FnDef
    Ptr<BaseType> currentType_;
    bool fromDecl_ = false;

    bool checkType(const BaseType *actual, const BaseType *expected);
    bool assertIsIntegerTy(const BaseType *type);

    void pushScope();
    void popScope();
    Ptr<BaseType> lookupType(const std::string &name) const;
    void insertType(const std::string &name, Ptr<BaseType> type);
};
} // namespace CodeGen
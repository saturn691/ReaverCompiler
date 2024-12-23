#pragma once

#include <optional>

#include "AST/Node.hpp"

namespace AST
{
// Forward declarations
class CompoundStmt;
class Expr;
class InitDeclList;
class TypeNode;

/**
 * Base class for declarations (including definitions and declarators)
 */
class Decl : public virtual BaseNode
{
public:
    virtual ~Decl() = default;
    virtual std::string getID() const = 0;
};

/**
 * Declaration node
 * e.g. `int a = 1;`
 */
class DeclNode final : public Node<DeclNode>, public Decl
{
public:
    DeclNode(const TypeNode *type);
    DeclNode(const TypeNode *type, const InitDeclList *decl);

    std::string getID() const override
    {
        return "";
    }

    std::vector<std::string> getIDs() const;

    Ptr<TypeNode> type_;
    Ptr<InitDeclList> initDeclList_;
};

/**
 * Function declarator
 * e.g. `foo(int a)
 */
class FnDecl final : public Node<FnDecl>, public Decl
{
public:
    FnDecl(const Decl *decl)
        : decl_(decl), params_(std::make_unique<ParamList>())
    {
    }

    FnDecl(const Decl *decl, const ParamList *params)
        : decl_(decl), params_(params)
    {
    }

    std::string getID() const override
    {
        return decl_->getID();
    }

    Ptr<Decl> decl_;
    Ptr<ParamList> params_;
};

/**
 * Function definition
 * e.g. `int foo(int a) { return a; }`
 */
class FnDef final : public Node<FnDef>, public Decl
{
public:
    FnDef(const TypeNode *retType, const Decl *decl, const CompoundStmt *body)
        : retType_(retType), decl_(decl), body_(body)
    {
    }

    std::string getID() const override
    {
        return decl_->getID();
    }

    Ptr<TypeNode> retType_;
    Ptr<Decl> decl_;
    Ptr<CompoundStmt> body_;
};

/**
 * Initializer declarator
 * e.g. `a = 1`
 */
class InitDecl final : public Node<InitDecl>, public Decl
{
public:
    InitDecl(const Decl *decl) : decl_(decl)
    {
    }
    InitDecl(const Decl *decl, const Expr *expr) : decl_(decl), expr_(expr)
    {
    }

    std::string getID() const override
    {
        return decl_->getID();
    }

    Ptr<Decl> decl_;
    Ptr<Expr> expr_;
};

/**
 * Initializer declarator list
 * e.g. `int a = 1, b = 2`
 */
class InitDeclList final : public NodeList<InitDecl>, public Node<InitDeclList>
{
public:
    using NodeList::NodeList;
};

/**
 * Parameter declaration
 * e.g. `int a`
 */
class ParamDecl final : public Node<ParamDecl>, public Decl
{
public:
    ParamDecl(const TypeNode *type) : type_(type)
    {
    }

    ParamDecl(const TypeNode *type, const Decl *decl) : type_(type), decl_(decl)
    {
    }

    std::string getID() const override
    {
        return decl_->getID();
    }

    Ptr<TypeNode> type_;
    Ptr<Decl> decl_;
};

/**
 * Parameter list
 * e.g. `int a, int b`
 */
class ParamList final : public NodeList<ParamDecl>, public Node<ParamList>
{
public:
    using NodeList::NodeList;
};

/**
 * Translation unit. Entry point of the program.
 * e.g. `int main() { return 0; }`
 */
class TranslationUnit final : public NodeList<DeclNode, FnDef>,
                              public Node<TranslationUnit>
{
public:
    using NodeList::NodeList;
};
} // namespace AST
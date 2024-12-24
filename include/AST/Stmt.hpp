#pragma once

#include "AST/Node.hpp"

namespace AST
{
// Forward declarations
class Expr;

class Stmt : public virtual BaseNode
{
public:
    virtual ~Stmt() = default;
};

class BlockItemList final : public NodeList<DeclNode, Stmt>,
                            public Node<BlockItemList>
{
public:
    using NodeList::NodeList;
};

class CompoundStmt final : public Node<CompoundStmt>, public Stmt
{
public:
    CompoundStmt() = default;
    CompoundStmt(BlockItemList *nodes) : nodes_(nodes)
    {
    }

    Ptr<BlockItemList> nodes_; // Optional
};

class ExprStmt final : public Node<ExprStmt>, public Stmt
{
public:
    ExprStmt() = default;
    ExprStmt(const Expr *expr) : expr_(expr)
    {
    }

    Ptr<Expr> expr_;
};

class For final : public Node<For>, public Stmt
{
public:
    For(const Stmt *init, const ExprStmt *cond, const Stmt *body)
        : init_(Ptr<Stmt>(init)), cond_(cond), body_(body)
    {
    }

    For(const Stmt *init,
        const ExprStmt *cond,
        const Expr *expr,
        const Stmt *body)
        : init_(Ptr<Stmt>(init)), cond_(cond), expr_(expr), body_(body)
    {
    }

    For(const DeclNode *init, const ExprStmt *cond, const Stmt *body)
        : init_(Ptr<DeclNode>(init)), cond_(cond), body_(body)
    {
    }

    For(const DeclNode *init,
        const ExprStmt *cond,
        const Expr *expr,
        const Stmt *body)
        : init_(Ptr<DeclNode>(init)), cond_(cond), expr_(expr), body_(body)
    {
    }

    std::variant<Ptr<Stmt>, Ptr<DeclNode>> init_;
    Ptr<ExprStmt> cond_;
    Ptr<Expr> expr_; // Optional
    Ptr<Stmt> body_;
};

class IfElse final : public Node<IfElse>, public Stmt
{
public:
    IfElse(const Expr *cond, const Stmt *thenStmt)
        : cond_(cond), thenStmt_(thenStmt)
    {
    }

    IfElse(const Expr *cond, const Stmt *thenStmt, const Stmt *elseStmt)
        : cond_(cond), thenStmt_(thenStmt), elseStmt_(elseStmt)
    {
    }

    Ptr<Expr> cond_;
    Ptr<Stmt> thenStmt_;
    Ptr<Stmt> elseStmt_;
};

class Return final : public Node<Return>, public Stmt
{
public:
    Return() = default;
    Return(const Expr *expr) : expr_(expr)
    {
    }

    Ptr<Expr> expr_;
};

class While final : public Node<While>, public Stmt
{
public:
    While(const Expr *cond, const Stmt *body) : cond_(cond), body_(body)
    {
    }

    Ptr<Expr> cond_;
    Ptr<Stmt> body_;
};

} // namespace AST
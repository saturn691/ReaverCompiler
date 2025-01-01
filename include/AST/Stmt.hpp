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

/**
 * Break statement
 * e.g. `break;`
 */
class Break final : public Node<Break>, public Stmt
{
public:
    Break() = default;
};

/**
 * Case statement
 * e.g. `case 1:` or `default:`
 */
class Case final : public Node<Case>, public Stmt
{
public:
    Case(const Stmt *body) : body_(body)
    {
    }
    Case(const Expr *expr, const Stmt *body) : expr_(expr), body_(body)
    {
    }

    Ptr<Expr> expr_; // Optional
    Ptr<Stmt> body_;
};

/**
 * Compound statement
 * e.g. `{ int a = 1; }`
 */
class CompoundStmt final : public Node<CompoundStmt>, public Stmt
{
public:
    CompoundStmt() = default;
    CompoundStmt(BlockItemList *nodes) : nodes_(nodes)
    {
    }

    Ptr<BlockItemList> nodes_; // Optional
};

/**
 * Continue statement
 * e.g. `continue;`
 */
class Continue final : public Node<Continue>, public Stmt
{
public:
    Continue() = default;
};

/**
 * Do-while statement
 * e.g. `do {} while (a < 10);`
 */
class DoWhile final : public Node<DoWhile>, public Stmt
{
public:
    DoWhile(const Stmt *body, const Expr *cond) : body_(body), cond_(cond)
    {
    }

    Ptr<Stmt> body_;
    Ptr<Expr> cond_;
};

/**
 * Expression statement
 * e.g. `a = 1;`
 */
class ExprStmt final : public Node<ExprStmt>, public Stmt
{
public:
    ExprStmt() = default;
    ExprStmt(const Expr *expr) : expr_(expr)
    {
    }

    Ptr<Expr> expr_; // Optional
};

/**
 * For statement
 * e.g. `for (int i = 0; i < 10; i++) {}`
 */
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

/**
 * If-else statement
 * e.g. `if (a < 10) {} else {}`
 */
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
    Ptr<Stmt> elseStmt_; // Optional
};

/**
 * Return statement
 * e.g. `return a;`
 */
class Return final : public Node<Return>, public Stmt
{
public:
    Return() = default;
    Return(const Expr *expr) : expr_(expr)
    {
    }

    Ptr<Expr> expr_; // Optional
};

/**
 * Switch statement
 * e.g. `switch (a) {}`
 */
class Switch final : public Node<Switch>, public Stmt
{
public:
    Switch(const Expr *expr, const Stmt *body) : expr_(expr), body_(body)
    {
    }

    Ptr<Expr> expr_;
    Ptr<Stmt> body_;
};

/**
 * While statement
 * e.g. `while (a < 10) {}`
 */
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
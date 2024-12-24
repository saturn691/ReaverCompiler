#pragma once

#include "AST/Decl.hpp"
#include "AST/Node.hpp"
#include "AST/Stmt.hpp"

namespace AST
{
class Expr : public Stmt
{
public:
    virtual ~Expr() = default;

    virtual bool isLValue() const
    {
        return false;
    }
};

/**
 * Argument expression list
 * e.g. `(1, 2)` in `foo(1, 2)`
 */
class ArgExprList final : public NodeList<Expr>, public Node<ArgExprList>
{
public:
    using NodeList::NodeList;
};

/**
 * Assignment node
 * e.g. `a = 1`
 */
class Assignment final : public Node<Assignment>, public Expr
{
public:
    enum class Op
    {
        ASSIGN,
        MUL_ASSIGN,
        DIV_ASSIGN,
        MOD_ASSIGN,
        ADD_ASSIGN,
        SUB_ASSIGN,
        LEFT_ASSIGN,
        RIGHT_ASSIGN,
        AND_ASSIGN,
        XOR_ASSIGN,
        OR_ASSIGN,
    };

    Assignment(const Expr *lhs, const Expr *rhs, Op op)
        : lhs_(lhs), rhs_(rhs), op_(op)
    {
    }

    Ptr<Expr> lhs_;
    Ptr<Expr> rhs_;
    Op op_;
};

/**
 * Binary operation
 * e.g. `a + b`
 */
class BinaryOp final : public Node<BinaryOp>, public Expr
{
public:
    enum class Op
    {
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,
        AND,
        OR,
        XOR,
        SHL,
        SHR,
        EQ,
        NE,
        LT,
        GT,
        LE,
        GE,
        LAND,
        LOR,
    };

    BinaryOp(const Expr *lhs, const Expr *rhs, Op op)
        : lhs_(lhs), rhs_(rhs), op_(op)
    {
    }

    Ptr<Expr> lhs_;
    Ptr<Expr> rhs_;
    Op op_;
};

/**
 * Constant
 * e.g. `10`, `0x12`, `1.1f`
 */
class Constant final : public Node<Constant>, public Expr
{
public:
    Constant(std::string value) : value_(std::move(value))
    {
    }

    std::string value_;
};

/**
 * Function call
 * e.g. `foo()`, `bar(1, 2)`
 */
class FnCall final : public Node<FnCall>, public Expr
{
public:
    FnCall(const Expr *fn) : fn_(fn)
    {
    }

    FnCall(const Expr *fn, const ArgExprList *args) : fn_(fn), args_(args)
    {
    }

    Ptr<Expr> fn_;
    Ptr<ArgExprList> args_;
};

/**
 * Identifier
 * e.g. `a`, `foo`
 */
class Identifier final : public Node<Identifier>, public Expr, public Decl
{
public:
    Identifier(std::string name) : name_(std::move(name))
    {
    }

    std::string getID() const override
    {
        return name_;
    }

    bool isLValue() const override
    {
        return true;
    }

    std::string name_;
};

} // namespace AST
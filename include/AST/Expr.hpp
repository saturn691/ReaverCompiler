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
};

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

class Constant final : public Node<Constant>, public Expr
{
public:
    Constant(std::string value) : value_(std::move(value))
    {
    }

    std::string value_;
};

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

    std::string name_;
};

} // namespace AST
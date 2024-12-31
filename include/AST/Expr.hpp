#pragma once

#include <optional>
#include <unordered_map>

#include "AST/Decl.hpp"
#include "AST/Node.hpp"
#include "AST/Stmt.hpp"
#include "AST/Type.hpp"

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

    virtual std::optional<int> eval() const
    {
        return std::nullopt;
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
 * Array access
 * e.g. `arr[0]`
 */
class ArrayAccess final : public Node<ArrayAccess>, public Expr
{
public:
    ArrayAccess(const Expr *arr, const Expr *index) : arr_(arr), index_(index)
    {
    }

    bool isLValue() const override
    {
        return true;
    }

    Ptr<Expr> arr_;
    Ptr<Expr> index_;
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

    std::optional<int> eval() const override
    {
        auto l = lhs_->eval();
        auto r = rhs_->eval();

        if (!l || !r)
        {
            return std::nullopt;
        }

        switch (op_)
        {
        case Op::ADD:
            return *l + *r;
        case Op::SUB:
            return *l - *r;
        case Op::MUL:
            return *l * *r;
        case Op::DIV:
            return *l / *r;
        case Op::MOD:
            return *l % *r;
        case Op::AND:
            return *l & *r;
        case Op::OR:
            return *l | *r;
        case Op::XOR:
            return *l ^ *r;
        case Op::SHL:
            return *l << *r;
        case Op::SHR:
            return *l >> *r;
        case Op::EQ:
            return *l == *r;
        case Op::NE:
            return *l != *r;
        case Op::LT:
            return *l < *r;
        case Op::GT:
            return *l > *r;
        case Op::LE:
            return *l <= *r;
        case Op::GE:
            return *l >= *r;
        case Op::LAND:
            return *l && *r;
        case Op::LOR:
            return *l || *r;
        }

        return std::nullopt;
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
    Constant(std::string value);

    std::optional<int> eval() const override
    {
        return std::stoi(value_);
    }

    char getChar() const;

    // Defined in C99 6.4.4.4 (Character constants)
    static const std::unordered_map<char, char> escapeChars;
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

class Paren final : public Node<Paren>, public Expr
{
public:
    Paren(const Expr *expr) : expr_(expr)
    {
    }

    bool isLValue() const override
    {
        // cppreference: The following expressions are lvalues ...
        // "parenthesized expression if the unparenthesized expression is an
        // lvalue"
        return expr_->isLValue();
    }

    std::optional<int> eval() const override
    {
        return expr_->eval();
    }

    Ptr<Expr> expr_;
};

class SizeOf final : public Node<SizeOf>, public Expr
{
public:
    SizeOf(const Expr *expr) : expr_(expr)
    {
    }

    SizeOf(const TypeDecl *type) : type_(type)
    {
    }

    Ptr<Expr> expr_;
    Ptr<TypeDecl> type_;
};

class StringLiteral final : public Node<StringLiteral>, public Expr
{
public:
    StringLiteral(std::string value) : originalValue_(value)
    {
        value_.reserve(value.size());

        for (size_t i = 1; i < value.size() - 1; i++)
        {
            if (value[i] == '\\')
            {
                i++;
                value_ += Constant::escapeChars.at(value[i]);
            }
            else
            {
                value_ += value[i];
            }
        }
    }

    std::string originalValue_;
    std::string value_;
};

class StructAccess final : public Node<StructAccess>, public Expr
{
public:
    StructAccess(const Expr *expr, std::string member)
        : expr_(expr), member_(member)
    {
    }

    bool isLValue() const override
    {
        return true;
    }

    Ptr<Expr> expr_;
    std::string member_;
};

class StructPtrAccess final : public Node<StructPtrAccess>, public Expr
{
public:
    StructPtrAccess(const Expr *expr, std::string member)
        : expr_(expr), member_(member)
    {
    }

    bool isLValue() const override
    {
        return true;
    }

    Ptr<Expr> expr_;
    std::string member_;
};

class UnaryOp final : public Node<UnaryOp>, public Expr
{
public:
    enum class Op
    {
        ADDR,
        DEREF,
        PLUS,
        MINUS,
        NOT,
        LNOT,
        POST_DEC,
        POST_INC,
        PRE_DEC,
        PRE_INC,
    };

    UnaryOp(const Expr *expr, Op op) : expr_(expr), op_(op)
    {
    }

    bool isLValue() const override
    {
        return op_ == Op::DEREF;
    }

    std::optional<int> eval() const override
    {
        auto e = expr_->eval();

        if (!e)
        {
            return std::nullopt;
        }

        switch (op_)
        {
        case Op::PLUS:
            return +*e;
        case Op::MINUS:
            return -*e;
        case Op::NOT:
            return ~*e;
        case Op::LNOT:
            return !*e;
        case Op::POST_DEC:
            return *e;
        case Op::POST_INC:
            return *e;
        case Op::PRE_DEC:
            return --*e;
        case Op::PRE_INC:
            return ++*e;
        default:
            return std::nullopt;
        }
    }

    Ptr<Expr> expr_;
    Op op_;
};

} // namespace AST
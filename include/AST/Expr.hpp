#pragma once

#include <limits>
#include <optional>
#include <unordered_map>

#include "AST/Decl.hpp"
#include "AST/Node.hpp"
#include "AST/Stmt.hpp"
#include "AST/Type.hpp"

namespace AST
{
struct EvalType
{
    EvalType();
    EvalType(double value);
    EvalType(uint64_t value);
    EvalType(int64_t value);
    EvalType(std::string value);

    std::optional<double> getDouble() const;
    std::optional<uint64_t> getUInt() const;
    std::optional<int64_t> getInt() const;
    std::optional<std::string> getString() const;
    operator bool() const;

    template <typename T>
    bool is() const
    {
        return std::holds_alternative<T>(value);
    }

    Types getType() const;

    std::variant<std::monostate, double, uint64_t, int64_t, std::string> value;
};

class Expr : public Stmt
{
public:
    virtual ~Expr() = default;

    virtual bool isLValue() const
    {
        return false;
    }

    virtual EvalType eval() const
    {
        return EvalType();
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

    BinaryOp(const Expr *lhs, const Expr *rhs, Op op);

    EvalType eval() const override;

    Ptr<Expr> lhs_;
    Ptr<Expr> rhs_;
    Op op_;
};

/**
 * Explicit cast expression
 * e.g. `(int) 1.1`
 */
class Cast final : public Node<Cast>, public Expr
{
public:
    Cast(const TypeDecl *type, const Expr *expr) : type_(type), expr_(expr)
    {
    }

    Ptr<TypeDecl> type_;
    Ptr<Expr> expr_;
};

/**
 * Constant
 * e.g. `10`, `0x12`, `1.1f`
 */
class Constant final : public Node<Constant>, public Expr
{
public:
    Constant(std::string value);

    EvalType eval() const override;

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
class Identifier final : public Node<Identifier>, public Decl, public Expr
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

/**
 * Initializer (intermediate node)
 * e.g. `1`, `{1, 2}`
 */
class Init final : public Node<Init>, public Expr
{
public:
    Init(const Expr *expr) : expr_(expr)
    {
    }

    Ptr<Expr> expr_;
};

/**
 * Initializer list
 * e.g. `{1, 2}`
 */
class InitList final : public NodeList<Init>, public Node<InitList>, public Expr
{
public:
    using NodeList::NodeList;
};

/**
 * Parenthesized expression
 * e.g. `(a + b)`
 */
class Paren final : public Node<Paren>, public Decl, public Expr
{
public:
    Paren(const Expr *expr) : expr_(expr)
    {
    }
    Paren(const Decl *decl) : decl_(decl)
    {
    }

    std::string getID() const override
    {
        if (decl_)
        {
            return decl_->getID();
        }
        return "";
    }

    bool isLValue() const override
    {
        if (expr_)
        {
            // cppreference: The following expressions are lvalues ...
            // "parenthesized expression if the unparenthesized expression is an
            // lvalue"
            return expr_->isLValue();
        }
        return false;
    }

    EvalType eval() const override
    {
        if (expr_)
        {
            return expr_->eval();
        }
        return {};
    }

    Ptr<Decl> decl_; // Optional
    Ptr<Expr> expr_; // Optional
};

/**
 * Sizeof expression
 * e.g. `sizeof(int)`, `sizeof a`
 */
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

/**
 * String literal
 * e.g. `"hello"`
 */
class StringLiteral final : public Node<StringLiteral>, public Expr
{
public:
    StringLiteral(std::string value);

    EvalType eval() const override;

    std::string originalValue_;
    std::string value_;
};

/**
 * Struct access
 * e.g. `foo.bar`
 */
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

/**
 * Struct pointer access
 * e.g. `foo->bar`
 */
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

/**
 * Ternary operation
 * e.g. `a ? b : c`
 */
class TernaryOp final : public Node<TernaryOp>, public Expr
{
public:
    TernaryOp(const Expr *cond, const Expr *lhs, const Expr *rhs)
        : cond_(cond), lhs_(lhs), rhs_(rhs)
    {
    }

    Ptr<Expr> cond_;
    Ptr<Expr> lhs_;
    Ptr<Expr> rhs_;
};

/**
 * Unary operation
 * e.g. `*a`, `!b`
 */
class UnaryOp final : public Node<UnaryOp>, public Expr
{
public:
    enum class Op
    {
        ADDR,     // &
        DEREF,    // *
        PLUS,     // +
        MINUS,    // -
        NOT,      // ~
        LNOT,     // !
        POST_DEC, // x--
        POST_INC, // x++
        PRE_DEC,  // --x
        PRE_INC,  // ++x
    };

    UnaryOp(const Expr *expr, Op op) : expr_(expr), op_(op)
    {
    }

    bool isLValue() const override
    {
        return op_ == Op::DEREF;
    }

    EvalType eval() const override;

    Ptr<Expr> expr_;
    Op op_;
};

} // namespace AST
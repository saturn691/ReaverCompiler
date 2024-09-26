#pragma once

/**
 * An expression is a sequence of operators and their operands, that specifies a
 * computation. Expressions have:
 *  - a type
 *  - a value (lvalue, rvalue or function designator).
 *
 * Lvalues can be modifiable or non-modifiable (if const etc.).
 *
 * For reference: https://en.cppreference.com/w/c/language/expressions
 */

#include <ast/models/ast_declaration.hpp>
#include <ast/models/ast_node.hpp>
#include <ast/models/ast_statement.hpp>
#include <ast/utils/ast_context.hpp>

#include <ir/models/ir_statement.hpp>

namespace ast
{
using ExprLowerR_t = std::unique_ptr<const ir::Rvalue>;
using ExprLowerL_t = std::unique_ptr<const ir::Lvalue>;

/**
 * Base class for all expressions.
 */
class Expression : public Node
{
public:
    virtual ~Expression() = default;

    virtual void print(std::ostream &dst, int indent_level) const override = 0;

    virtual Types_t get_type(Context &context) const = 0;

    // Entry point (from Statement)
    virtual ExprLowerR_t lower(Context &context,
        const std::unique_ptr<ir::BasicBlock> &block,
        const std::optional<ir::Lvalue> &dest) const = 0;

    virtual ExprLowerL_t lower(Context &context) const = 0;
};

enum class AssignmentType
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
    OR_ASSIGN
};

/**
 * An assignment operation, including compound assignments.
 * Assignments are right-associative (e.g. `a = b = c` = `a = (b = c)`).
 * The left-hand side must be an lvalue (x, *x, x[i], x.y, x->y).
 *
 * e.g. `a = b`, `a += b`, etc.
 */
class Assignment : public Expression
{
public:
    Assignment(const Expression *lhs,
        const AssignmentType op,
        const Expression *rhs);

    void print(std::ostream &dst, int indent_level) const override;

    Types_t get_type(Context &context) const override;

    ExprLowerR_t lower(Context &context,
        const std::unique_ptr<ir::BasicBlock> &block,
        const std::optional<ir::Lvalue> &dest) const override;

    ExprLowerL_t lower(Context &context) const override;

private:
    std::unique_ptr<const Expression> lhs;
    AssignmentType op;
    std::unique_ptr<const Expression> rhs;
};

enum class BinaryOpType
{
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    BITWISE_AND,
    BITWISE_OR,
    BITWISE_XOR,
    LSL,
    LSR,
    EQ,
    NE,
    LT,
    GT,
    LE,
    GE,
    LOGICAL_AND,
    LOGICAL_OR
};

/**
 * A binary operation. Includes any operation with two operands.
 * e.g. `a + b`, `a & b`, `a == b`, etc.
 */
class BinaryOp : public Expression
{
public:
    BinaryOp(const Expression *left,
        const Expression *right,
        const BinaryOpType op);

    void print(std::ostream &dst, int indent_level) const override;

    static ir::BinaryOpType to_ir_type(BinaryOpType op);

    Types_t get_type(Context &context) const override;

    ExprLowerR_t lower(Context &context,
        const std::unique_ptr<ir::BasicBlock> &block,
        const std::optional<ir::Lvalue> &dest) const override;

    ExprLowerL_t lower(Context &context) const override;

private:
    std::unique_ptr<const Expression> left;
    std::unique_ptr<const Expression> right;
    BinaryOpType op;
};

/**
 * Constants
 * e.g. `1.0f`, `1.0`, `1`
 */
class Constant : public Expression

{
public:
    Constant(const std::string value);

    void print(std::ostream &dst,
        [[maybe_unused]] int indent_level) const override;

    Types_t get_type(Context &context) const override;

    ExprLowerR_t lower(Context &context,
        const std::unique_ptr<ir::BasicBlock> &block,
        const std::optional<ir::Lvalue> &dest) const override;

    ExprLowerL_t lower(Context &context) const override;

private:
    const std::string value;
};

/**
 * Identifiers.
 * e.g. `foo`, `bar`, etc.
 */
class Identifier : virtual public Expression, virtual public Declarator
{
public:
    Identifier(const std::string &id);

    void print(std::ostream &dst, int indent_level) const override;

    Types_t get_type(Context &context) const override;

    std::string get_id() const override;

    ExprLowerR_t lower(Context &context,
        const std::unique_ptr<ir::BasicBlock> &block,
        const std::optional<ir::Lvalue> &dest) const override;

    ExprLowerL_t lower(Context &context) const override;

private:
    std::string id;
};
} // namespace ast

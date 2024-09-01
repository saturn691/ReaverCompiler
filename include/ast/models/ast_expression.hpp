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

#include <ast/models/ast_node.hpp>
#include <ast/models/ast_statement.hpp>
#include <ast/models/ast_declaration.hpp>
#include <ast/utils/ast_context.hpp>

#include <ir/models/ir_statement.hpp>

namespace ast
{
    using Dest = std::optional<ir::Declaration>;
    using ExprLower_t = std::unique_ptr<const ir::Rvalue>;

    /**
     * Base class for all expressions.
     */
    class Expression : public Node
    {
    public:
        virtual ~Expression() = default;

        virtual void print(std::ostream &dst, int indent_level) const override = 0;

        virtual Types_t get_type(Context &context) const = 0;

        virtual ExprLower_t lower(
            Context &context,
            const ir::FunctionHeader &header,
            ir::FunctionLocals &locals,
            const std::unique_ptr<ir::BasicBlock> &block,
            const Dest dest) const = 0;
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
        BinaryOp(
            const Expression *left,
            const Expression *right,
            const BinaryOpType op);

        void print(std::ostream &dst, int indent_level) const override;

        static ir::BinaryOpType to_ir_type(BinaryOpType op);

        Types_t get_type(Context &context) const override;

        ExprLower_t lower(
            Context &context,
            const ir::FunctionHeader &header,
            ir::FunctionLocals &locals,
            const std::unique_ptr<ir::BasicBlock> &block,
            const Dest dest) const override;

    private:
        std::unique_ptr<const Expression> left;
        std::unique_ptr<const Expression> right;
        BinaryOpType op;
    };

    /**
     * Identifiers.
     * e.g. `foo`, `bar`, etc.
     */
    class Identifier : virtual public Expression,
                       virtual public Declarator
    {
    public:
        Identifier(const std::string &id);

        void print(std::ostream &dst, int indent_level) const override;

        Types_t get_type(Context &context) const override;

        std::string get_id() const override;

        ExprLower_t lower(
            Context &context,
            const ir::FunctionHeader &header,
            ir::FunctionLocals &locals,
            const std::unique_ptr<ir::BasicBlock> &block,
            const Dest dest) const override;

    private:
        std::string id;
    };
}

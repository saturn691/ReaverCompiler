#pragma once

/**
 * Statements are fragments of the C program that are executed in sequence.
 *
 * For reference: https://en.cppreference.com/w/c/language/statements
 */

#include <memory>

#include <ast/models/ast_node.hpp>
#include <ast/models/ast_node_list.hpp>

#include <ir/models/ir_function.hpp>
#include <ir/models/ir_basic_block.hpp>

namespace ast
{
    // Forward declaration
    class DeclarationList;
    class Expression;

    using ExprLowerR_t = std::unique_ptr<const ir::Rvalue>;

    /**
     * Base class for all statements.
     */
    class Statement : public Node
    {
    public:
        virtual void print(std::ostream &dst, int indent_level) const override = 0;

        virtual void lower(
            Context &context,
            const ir::FunctionHeader &header,
            ir::BasicBlocks &bbs) const = 0;
    };

    /**
     * A list of statements.
     */
    class StatementList : public NodeList<Statement>
    {
    public:
        using NodeList::NodeList;

        void print(std::ostream &dst, int indent_level) const override;
    };

    /**
     * A sequence of statements in braces. Creates a new scope.
     * e.g. `{ int x = 0; x++; }`
     */
    class CompoundStatement : public Statement
    {
    public:
        CompoundStatement() = default;

        CompoundStatement(
            const DeclarationList *decls,
            const StatementList *stmts);

        void print(std::ostream &dst, int indent_level) const override;

        // The compound statement of a function definition
        void lower(
            Context &context,
            const ir::FunctionHeader &header,
            ir::BasicBlocks &bbs) const override;

    private:
        std::unique_ptr<const DeclarationList> decls;
        std::unique_ptr<const StatementList> stmts;
    };

    class ExpressionStatement : public Statement
    {
    public:
        ExpressionStatement() = default;

        ExpressionStatement(const Expression *expr);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(
            Context &context,
            const ir::FunctionHeader &header,
            ir::BasicBlocks &bbs) const override;

    private:
        std::unique_ptr<const Expression> expr;
    };

    /**
     * A return statement, with an optional expression.
     * e.g. `return 0;`
     */
    class Return : public Statement
    {
    public:
        Return() = default;

        Return(const Expression *expr);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(
            Context &context,
            const ir::FunctionHeader &header,
            ir::BasicBlocks &bbs) const override;

    private:
        std::unique_ptr<const Expression> expr;
    };
}

#pragma once

/**
 * Statements are fragments of the C program that are executed in sequence.
 *
 * For reference: https://en.cppreference.com/w/c/language/statements
 */

#include <memory>

#include <ast/models/ast_node.hpp>
#include <ast/models/ast_node_list.hpp>

#include <ir/models/ir_basic_block.hpp>
#include <ir/models/ir_function.hpp>

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

    virtual void lower(Context &context,
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

    CompoundStatement(const DeclarationList *decls, const StatementList *stmts);

    void print(std::ostream &dst, int indent_level) const override;

    // The compound statement of a function definition
    void lower(Context &context,
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

    void lower(Context &context,
        const ir::FunctionHeader &header,
        ir::BasicBlocks &bbs) const override;

    ExprLowerR_t lower(Context &context,
        const std::unique_ptr<ir::BasicBlock> &block,
        const std::optional<ir::Lvalue> &dest) const;

private:
    std::unique_ptr<const Expression> expr;
};

/**
 * An if-else statement (else optional)
 * e.g. `if (1) {} else {}`
 */
class If : public Statement
{
public:
    If(const Expression *condition, const Statement *statement);

    If(const Expression *condition,
        const Statement *statement,
        const Statement *else_statement);

    void print(std::ostream &dst, int indent_level) const override;

    void lower(Context &context,
        const ir::FunctionHeader &header,
        ir::BasicBlocks &bbs) const override;

private:
    std::unique_ptr<const Expression> condition;
    std::unique_ptr<const Statement> statement;
    std::unique_ptr<const Statement> else_statement;
};

/**
 * A for statement
 * e.g. `for (i = 0; i < 2; i++) {}`
 */
class For : public Statement
{
public:
    For(const ExpressionStatement *init,
        const ExpressionStatement *cond,
        const Statement *stmt);

    For(const ExpressionStatement *init,
        const ExpressionStatement *cond,
        const Expression *loop,
        const Statement *stmt);

    void print(std::ostream &dst, int indent_level) const override;

    void lower(Context &context,
        const ir::FunctionHeader &header,
        ir::BasicBlocks &bbs) const override;

private:
    std::unique_ptr<const ExpressionStatement> init;
    std::unique_ptr<const ExpressionStatement> cond;
    std::unique_ptr<const Expression> loop;
    std::unique_ptr<const Statement> stmt;
};

/**
 * A while statement
 * e.g. `while (1) {}`
 */
class While : public Statement
{
public:
    While(const Expression *condition, const Statement *statement);

    void print(std::ostream &dst, int indent_level) const override;

    void lower(Context &context,
        const ir::FunctionHeader &header,
        ir::BasicBlocks &bbs) const override;

private:
    std::unique_ptr<const Expression> condition;
    std::unique_ptr<const Statement> statement;
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

    void lower(Context &context,
        const ir::FunctionHeader &header,
        ir::BasicBlocks &bbs) const override;

private:
    std::unique_ptr<const Expression> expr;
};
} // namespace ast

#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

/**
 * The purpose of this node is to flush the buffer (std::endl) after a
 * statement is printed.
 */
namespace ast
{
    class Statement : public Node
    {
    public:
        Statement(const Node *statement);

        void print(std::ostream &dst, int indent_level) const override;

        template <typename... Args>
        void lower(Context &context, Args &&...args) const;

    private:
        std::unique_ptr<const Node> statement;
    };
}
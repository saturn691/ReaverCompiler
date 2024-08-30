#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

namespace ast
{
    class If : public Node
    {
    public:
        If(
            const Node *condition,
            const Node *statement);

        If(
            const Node *condition,
            const Node *statement,
            const Node *else_statement);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const;

    private:
        std::shared_ptr<const Node> condition;
        std::shared_ptr<const Node> statement;
        std::shared_ptr<const Node> else_statement;
    };
}

#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

namespace ast
{
    class Switch : public Node
    {
    public:
        Switch(const Node *expression, const Node *statement);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const override;

    private:
        std::unique_ptr<const Node> expression;
        std::unique_ptr<const Node> statement;
    };
}

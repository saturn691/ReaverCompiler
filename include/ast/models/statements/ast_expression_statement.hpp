#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

namespace ast
{
    class ExpressionStatement : public Node
    {
    public:
        ExpressionStatement();

        ExpressionStatement(const Node *expression);

        void print(std::ostream &dst, int indent_level) const override;

        template <typename... Args>
        void lower(Context &context, Args &&...args) const;

    private:
        std::unique_ptr<const Node> expression;
    };
}

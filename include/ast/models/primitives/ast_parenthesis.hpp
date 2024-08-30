#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

namespace ast
{
    class Parenthesis : public Node
    {
    public:
        Parenthesis(const Node *expr);

        virtual void print(std::ostream &dst, int indent_level) const override;

        template <typename... Args>
        decltype(auto) lower(Context &context, Args &&...args) const;

    private:
        std::shared_ptr<const Node> expr;
    };
}

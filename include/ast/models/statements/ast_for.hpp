#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

namespace ast
{
    class For : public Node
    {
    public:
        For(
            const Node *init,
            const Node *condition,
            const Node *statement);

        For(
            const Node *init,
            const Node *condition,
            const Node *iteration,
            const Node *statement);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const;

    private:
        std::shared_ptr<const Node> init;
        std::shared_ptr<const Node> condition;
        std::shared_ptr<const Node> iteration;
        std::shared_ptr<const Node> statement;
    };
}

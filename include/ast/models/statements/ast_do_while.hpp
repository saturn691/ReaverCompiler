#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

namespace ast
{
    class DoWhile : public Node
    {
    public:
        DoWhile(const Node *statement, const Node *condition);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const override;

    private:
        std::unique_ptr<const Node> statement;
        std::unique_ptr<const Node> condition;
    };
}

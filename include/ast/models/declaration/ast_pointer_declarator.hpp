#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

namespace ast
{
    class PointerDeclarator : public Node
    {
    public:
        PointerDeclarator(int amount, const Node *declarator);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const;

    private:
        int amount;
        std::unique_ptr<const Node> declarator;
    };
}

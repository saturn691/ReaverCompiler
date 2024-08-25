#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

namespace ast
{
    class ArrayDeclarator : public Node
    {
    public:
        ArrayDeclarator(const Node *declarator);

        ArrayDeclarator(const Node *declarator, const Node *size);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const override;

    private:
        std::unique_ptr<const Node> declarator;
        std::unique_ptr<const Node> size;
    };
}

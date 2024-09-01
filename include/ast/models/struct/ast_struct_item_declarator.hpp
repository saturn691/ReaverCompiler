#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

namespace ast
{
    class StructItemDeclarator : public Node
    {
    public:
        StructItemDeclarator(
            const Node *declarator);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const;

    private:
        std::unique_ptr<const Node> declarator;
    };
}

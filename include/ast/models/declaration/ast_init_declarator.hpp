#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

namespace ast
{
    class InitDeclarator : public Node
    {
    public:
        InitDeclarator(const Node *declarator, const Node *initializer);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const;

    private:
        std::unique_ptr<const Node> declarator;
        std::unique_ptr<const Node> initializer;
    };
}

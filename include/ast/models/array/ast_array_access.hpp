#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

namespace ast
{
    class ArrayAccess : public Node
    {
    public:
        ArrayAccess(const Node *array, const Node *expr);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const;

    private:
        std::unique_ptr<const Node> array;
        std::unique_ptr<const Node> expr;
    };
}

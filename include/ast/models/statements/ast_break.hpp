#pragma once

#include <ast/models/ast_node.hpp>

namespace ast
{
    class Break : public Node
    {
    public:
        Break();

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const override;
    };
}

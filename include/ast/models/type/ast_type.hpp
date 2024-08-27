#pragma once

#include <ast/models/ast_node.hpp>

#include <ir/models/ir_type.hpp>

namespace ast
{
    class Type : public Node
    {
    public:
        virtual unsigned int get_size() const = 0;

        virtual ir::Type lower(Context &context) const = 0;
    };
}

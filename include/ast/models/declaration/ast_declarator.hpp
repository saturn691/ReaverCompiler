#pragma once

#include <memory>
#include <vector>

#include <ast/models/ast_node.hpp>

#include <ir/models/ir_local_declaration.hpp>

namespace ast
{
    class Declarator : public Node
    {
    public:
        virtual ~Declarator() = default;

        virtual void print(std::ostream &dst, int indent_level) const override = 0;

        virtual std::string get_id() const = 0;

        virtual std::vector<ir::Declaration> lower(Context &context) const = 0;
    };
}

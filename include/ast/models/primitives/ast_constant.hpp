#pragma once

#include <ast/models/ast_node.hpp>

namespace ast
{
    class Constant : public Node
    {
    public:
        Constant(const std::string value);

        virtual void print(std::ostream &dst, int indent_level) const override;

        virtual void lower(Context &context) const override;

    private:
        std::string value;
    };
}

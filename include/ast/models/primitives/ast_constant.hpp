#pragma once

#include <ast/models/ast_node.hpp>

namespace ast
{
    class Constant : public Node
    {
    public:
        Constant(const std::string value);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const;

    private:
        std::string value;
    };
}

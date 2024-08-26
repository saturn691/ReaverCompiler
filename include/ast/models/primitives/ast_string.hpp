#pragma once

#include <ast/models/ast_node.hpp>

namespace ast
{
    class String : public Node
    {
    public:
        String(const std::string value);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const;

    private:
        std::string value;
    };
}

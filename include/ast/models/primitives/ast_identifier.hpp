#pragma once

#include <string>

#include <ast/models/ast_node.hpp>

namespace ast
{
    class Identifier : public Node
    {
    public:
        Identifier(const std::string id);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const;

    private:
        std::string id;
    };
}

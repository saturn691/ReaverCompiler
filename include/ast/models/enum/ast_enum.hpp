#pragma once

#include <ast/models/ast_node.hpp>
#include <ast/models/type/ast_type.hpp>

namespace ast
{
    class Enum : public Node, public Type
    {
    public:
        Enum(const std::string identifier);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const;

        unsigned int get_size() const override;

    private:
        std::string identifier;
    };
}

#pragma once

#include <ast/models/ast_node.hpp>
#include <ast/models/type/ast_type.hpp>

namespace ast
{
    class Enum : public Type
    {
    public:
        Enum(const std::string identifier);

        void print(std::ostream &dst, int indent_level) const override;

        ir::Type lower(Context &context) const override;

        unsigned int get_size() const override;

    private:
        std::string identifier;
    };
}

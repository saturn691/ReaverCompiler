#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>
#include <ast/models/type/ast_type.hpp>

enum class StructOrUnionType
{
    STRUCT,
    UNION
};

namespace ast
{
    class Struct : public Type
    {
    public:
        Struct(
            const StructOrUnionType type,
            const std::string identifier);

        void print(std::ostream &dst, int indent_level) const override;

        unsigned int get_size() const override;

    private:
        StructOrUnionType type;
        std::string identifier;
    };
}

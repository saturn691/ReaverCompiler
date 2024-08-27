#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>
#include <ast/models/ast_node_list.hpp>
#include <ast/models/type/ast_type.hpp>
#include <ast/models/enum/ast_enum_list.hpp>

namespace ast
{
    class EnumDeclaration : public Type
    {
    public:
        EnumDeclaration(const EnumList *enum_list);

        EnumDeclaration(
            const std::string identifier,
            const EnumList *enum_list);

        void print(std::ostream &dst, int indent_level) const override;

        ir::Type lower(Context &context) const override;

        unsigned int get_size() const override;

    private:
        std::string identifier;
        std::unique_ptr<const EnumList> enum_list;
    };
}

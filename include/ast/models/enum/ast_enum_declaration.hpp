#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>
#include <ast/models/ast_node_list.hpp>
#include <ast/models/type/ast_type.hpp>

namespace ast
{
    class EnumDeclaration : public Node, public Type
    {
    public:
        EnumDeclaration(const NodeList *enum_list);

        EnumDeclaration(
            const std::string identifier,
            const NodeList *enum_list);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const override;

        unsigned int get_size() const override;

    private:
        std::string identifier;
        std::unique_ptr<const NodeList> enum_list;
    };
}

#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>
#include <ast/models/ast_node_list.hpp>
#include <ast/models/struct/ast_struct_declaration_list.hpp>
#include <ast/models/struct/ast_struct.hpp>

namespace ast
{
    class StructDeclaration : public Type, public Node
    {
    public:
        // Anonymous struct/union
        StructDeclaration(
            const StructOrUnionType type,
            const StructDeclarationList *declarations);

        StructDeclaration(
            const StructOrUnionType type,
            const std::string identifier,
            const StructDeclarationList *declarations);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const;

        unsigned int get_size() const override;

    private:
        StructOrUnionType type;
        std::string identifier;
        std::unique_ptr<const StructDeclarationList> declarations;
    };
}

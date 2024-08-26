#pragma once

#include <ast/models/struct/ast_struct_item.hpp>
#include <ast/models/ast_node_list.hpp>

namespace ast
{
    class StructDeclarationList : public NodeList<StructItem>
    {
    public:
        using NodeList::NodeList;

        void print(std::ostream &dst, int indent_level) const override;
    };
}

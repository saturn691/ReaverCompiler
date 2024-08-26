#pragma once

#include <ast/models/ast_node_list.hpp>
#include <ast/models/struct/ast_struct_item_declarator.hpp>

namespace ast
{
    class StructItemList : public NodeList<StructItemDeclarator>
    {
    public:
        using NodeList::NodeList;

        void print(std::ostream &dst, int indent_level) const override;
    };
}

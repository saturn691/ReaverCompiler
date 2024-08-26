#pragma once

#include <ast/models/enum/ast_enum_item.hpp>
#include <ast/models/ast_node_list.hpp>

namespace ast
{
    class EnumList : public NodeList<EnumItem>
    {
    public:
        using NodeList::NodeList;

        void print(std::ostream &dst, int indent_level) const override;
    };
}

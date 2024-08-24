#pragma once

#include <ast/models/ast_node_list.hpp>

class EnumList : public NodeList
{
public:
    using NodeList::NodeList;

    void print(std::ostream &dst, int indent_level) const override;
};

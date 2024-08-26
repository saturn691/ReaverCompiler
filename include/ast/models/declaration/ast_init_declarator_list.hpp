#pragma once

#include <ast/models/ast_node_list.hpp>

namespace ast
{
    class InitDeclaratorList : public NodeList<Node>
    {
    public:
        using NodeList::NodeList;

        void print(std::ostream &dst, int indent_level) const override;
    };
}

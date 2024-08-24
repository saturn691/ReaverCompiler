#pragma once

#include <memory>

#include <ast/models/ast_node_list.hpp>

class FunctionParamList : public NodeList
{
public:
    using NodeList::NodeList;

    void print(std::ostream &dst, int indent_level) const override;
};

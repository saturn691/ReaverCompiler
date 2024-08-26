#pragma once

#include <memory>

#include <ast/models/function/ast_function_param.hpp>
#include <ast/models/ast_node_list.hpp>

namespace ast
{
    class FunctionParamList : public NodeList<FunctionParam>
    {
    public:
        using NodeList::NodeList;

        void print(std::ostream &dst, int indent_level) const override;
    };
}

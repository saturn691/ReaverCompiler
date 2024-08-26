#pragma once

#include <memory>

#include <ast/models/function/ast_function_param_list.hpp>
#include <ast/models/ast_node.hpp>
#include <ast/models/ast_node_list.hpp>

namespace ast
{
    class FunctionDeclarator : public Node
    {
    public:
        FunctionDeclarator(const Node *declarator);

        FunctionDeclarator(const Node *declarator, const FunctionParamList *params);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const;

    private:
        std::unique_ptr<const Node> declarator;
        std::unique_ptr<const FunctionParamList> params;
    };
}

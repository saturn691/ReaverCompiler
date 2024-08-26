#pragma once

#include <memory>

#include <ast/models/function/ast_function_call_list.hpp>
#include <ast/models/ast_node.hpp>
#include <ast/models/ast_node_list.hpp>

namespace ast
{
    class FunctionCall : public Node
    {
    public:
        FunctionCall(const Node *declarator, const FunctionCallList *args);

        virtual void print(std::ostream &dst, int indent_level) const override;

        virtual void lower(Context &context) const;

    private:
        std::unique_ptr<const Node> declarator;
        std::unique_ptr<const FunctionCallList> args;
    };
}

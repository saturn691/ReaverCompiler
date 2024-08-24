#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>
#include <ast/models/ast_node_list.hpp>

class FunctionCall : public Node
{
public:
    FunctionCall(const Node *declarator, const NodeList *args);

    virtual void print(std::ostream &dst, int indent_level) const override;

    virtual void lower(Context &context) const override;

private:
    std::unique_ptr<const Node> declarator;
    std::unique_ptr<const NodeList> args;
};

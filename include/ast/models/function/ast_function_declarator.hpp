#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>
#include <ast/models/ast_node_list.hpp>

class FunctionDeclarator : public Node
{
public:
    FunctionDeclarator(const Node *declarator);

    FunctionDeclarator(const Node *declarator, const NodeList *params);

    void print(std::ostream &dst, int indent_level) const override;

    void lower(Context &context) const override;

private:
    std::unique_ptr<const Node> declarator;
    std::unique_ptr<const NodeList> params;
};

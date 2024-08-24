#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

class ExpressionStatement : public Node
{
public:
    ExpressionStatement();

    ExpressionStatement(const Node *expression);

    void print(std::ostream &dst, int indent_level) const override;

    void lower(Context &context) const override;

private:
    std::unique_ptr<const Node> expression;
};

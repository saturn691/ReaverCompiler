#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

class Parenthesis : public Node
{
public:
    Parenthesis(const Node *expr);

    virtual void print(std::ostream &dst, int indent_level) const override;

    virtual void lower(Context &context) const override;

private:
    std::unique_ptr<const Node> expr;
};

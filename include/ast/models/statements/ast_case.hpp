#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

class Case : public Node
{
public:
    Case(const Node *expression, const Node *statement);

    virtual void print(std::ostream &dst, int indent_level) const override;

    virtual void lower(Context &context) const override;

protected:
    std::unique_ptr<const Node> statement;

private:
    // constant expression
    std::unique_ptr<const Node> expression;
};

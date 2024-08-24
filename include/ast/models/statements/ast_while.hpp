#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

class While : public Node
{
public:
    While(const Node *condition, const Node *statement);

    void print(std::ostream &dst, int indent_level) const override;

    void lower(Context &context) const override;

private:
    std::unique_ptr<const Node> condition;
    std::unique_ptr<const Node> statement;
};

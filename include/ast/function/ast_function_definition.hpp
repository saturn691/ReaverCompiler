#pragma once

#include <memory>

#include <ast/ast_node.hpp>

class FunctionDefinition : Node
{
public:
    FunctionDefinition(
        const Node *specifiers,
        const Node *declarator,
        const Node *statement)
        : specifiers(std::unique_ptr<const Node>(specifiers)),
          declarator(std::unique_ptr<const Node>(declarator)),
          statement(std::unique_ptr<const Node>(statement))
    {
    }

    void print(std::ostream &dst, int indent_level) const override
    {
    }

    void lower(Context &context) const override
    {
    }

private:
    std::unique_ptr<const Node> specifiers;
    std::unique_ptr<const Node> declarator;
    std::unique_ptr<const Node> statement;
};

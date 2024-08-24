#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

class EnumItem : public Node
{
public:
    EnumItem(const std::string identifier);

    EnumItem(
        const std::string identifier,
        const Node *expr);

    void print(std::ostream &dst, int indent_level) const override;

    void lower(Context &context) const override;

private:
    std::string identifier;
    std::unique_ptr<const Node> expr;
};

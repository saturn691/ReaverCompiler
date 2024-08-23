#pragma once

#include <string>

#include <ast/models/ast_node.hpp>

class Identifier : public Node
{
public:
    Identifier(std::string id);

    void print(std::ostream &dst, int indent_level) const override;

    void lower(Context &context) const override;

private:
    std::string id;
};

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include <ast/models/ast_node.hpp>

class NodeList : public Node
{
public:
    NodeList();

    NodeList(const Node *node);

    void push_back(const Node *node);

    virtual void print(std::ostream &dst, int indent_level) const override;

    virtual void lower(Context &context) const override;

protected:
    std::vector<std::unique_ptr<const Node>> nodes;

    void print_delim(
        std::ostream &dst,
        int indent_level,
        const std::string &delim) const;
};

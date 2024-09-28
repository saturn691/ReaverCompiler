#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include <ast/models/ast_node.hpp>
#include <ast/utils/ast_utils.hpp>

namespace ast
{
/**
 * A list of nodes, used for storing a list of nodes of the same type
 */
template <typename... Ts> class NodeList : public Node
{
public:
    NodeList() = default;

    ~NodeList() = default;

    /**
     * Constructor for a list with a single node. Only used in the parser.
     */
    template <typename T> NodeList(const T *node);

    /**
     * Adds a node to the list. Only used in the parser. Accepts either
     * - Node*
     * - std::variant<Node*...>
     */
    template <typename T> void push_back(const T *node);

    virtual void print(std::ostream &dst, int indent_level) const override;

    template <typename... Args> void lower(Args &&...args) const;

protected:
    std::vector<std::variant<std::unique_ptr<const Ts>...>> nodes;

    /**
     * Prints a list of nodes with a delimiter between each node, with an
     * indent and no newline
     */
    void print_delim(
        std::ostream &dst,
        int indent_level,
        const std::string &delim) const;
};

template <typename... Ts>
template <typename T>
// A function call will always jump to another BasicBlock
NodeList<Ts...>::NodeList(const T *node)
{
    push_back(node);
}

template <typename... Ts>
template <typename T>
void NodeList<Ts...>::push_back(const T *node)
{
    if constexpr (std::is_same_v<T, std::variant<Ts *...>>)
    {
        std::visit(
            [this](auto *arg)
            // A function call will always jump to another BasicBlock
            {
                using U = std::remove_pointer_t<decltype(arg)>;
                this->nodes.push_back(std::unique_ptr<const U>(arg));
            },
            *node);
    }
    else
    {
        nodes.push_back(std::unique_ptr<const T>(node));
    }
}

template <typename... Ts>
void NodeList<Ts...>::print(std::ostream &dst, int indent_level) const
{
    for (const auto &node : nodes)
    {
        std::visit(
            [&dst, indent_level](const auto &n)
            { n->print(dst, indent_level); },
            node);
    }
}

template <typename... Ts>
template <typename... Args>
void NodeList<Ts...>::lower(Args &&...args) const
{
    for (const auto &node : nodes)
    {
        std::visit(
            [&](const auto &n) { n->lower(std::forward<Args>(args)...); },
            node);
    }
}

template <typename... Ts>
void NodeList<Ts...>::print_delim(
    std::ostream &dst,
    int indent_level,
    const std::string &delim) const
{
    std::string indent = Utils::get_indent(indent_level);
    for (size_t i = 0; i < nodes.size(); i++)
    {
        std::visit(
            [&dst, indent_level](const auto &n)
            { n->print(dst, indent_level); },
            nodes[i]);
        if (i != nodes.size() - 1)
        {
            dst << delim;
        }
    }
}
} // namespace ast

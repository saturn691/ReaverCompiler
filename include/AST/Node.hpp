#pragma once

#include <iostream>
#include <memory>
#include <variant>
#include <vector>

#include "AST/Visitor.hpp"

namespace AST
{
class BaseNode;
using NodePtr = std::unique_ptr<const BaseNode>;

template <typename T>
using Ptr = std::unique_ptr<const T>;

/**
 *  Base class for nodes in the AST. Only contains virtual functions.
 *  This is the standard interface for all nodes and only contains the functions
 *  that all nodes must implement.
 */
class BaseNode
{
public:
    virtual ~BaseNode() = default;
    virtual void accept(Visitor &visitor) const = 0;
};

/**
 * CRTP class for nodes in the AST
 */
template <typename Derived>
class Node : public virtual BaseNode
{
public:
    virtual ~Node() = default;

    void accept(Visitor &visitor) const override
    {
        visitor.visit(static_cast<const Derived &>(*this));
    }

private:
    Node() = default;
    friend Derived;
};

/**
 * Base class for nodes that contain a list of nodes
 */
template <typename... Ts>
class NodeList
{
public:
    NodeList() = default;

    template <typename T>
    NodeList(const T *node)
    {
        pushBack(node);
    }

    ~NodeList() = default;

    template <typename T>
    void pushBack(const T *node)
    {
        if constexpr (std::is_same_v<T, std::variant<Ts *...>>)
        {
            std::visit(
                [this](auto *arg)
                {
                    using U = std::remove_pointer_t<decltype(arg)>;
                    nodes_.push_back(std::unique_ptr<const U>(arg));
                },
                *node);
        }
        else
        {
            nodes_.push_back(std::unique_ptr<const T>(node));
        }
    }

    std::vector<std::variant<std::unique_ptr<const Ts>...>> nodes_;
};

// Helper class to visit multiple lambdas
template <class... Ts>
struct overloads : Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
overloads(Ts...) -> overloads<Ts...>;

} // namespace AST
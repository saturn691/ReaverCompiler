#pragma once

#include <string>
#include <iostream>

#include <ast/utils/ast_context.hpp>

/**
 *  Base class for nodes in the AST. Only contains virtual functions.
 *  This is the standard interface for all nodes and only contains the functions
 *  that all nodes must implement.
 */
namespace ast
{
    class Node
    {
    public:
        virtual ~Node() = default;

        virtual void print(std::ostream &dst, int indent_level) const = 0;

        // Turns the AST node to IR (never called - just a placeholder)
        template <typename... Args>
        decltype(auto) lower(Context &context, Args &&...args) const;
    };
}

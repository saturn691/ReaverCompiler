#pragma once

#include <string>
#include <iostream>

#include <ast/ast_context.hpp>

/**
 *  Base class for nodes in the AST. Only contains virtual functions.
 *  This is the standard interface for all nodes and only contains the functions
 *  that all nodes must implement.
 */
class Node
{
public:
    virtual ~Node()
    {
    }

    // Tell and expression to print itself to the given stream
    virtual void print(std::ostream &dst, int indent_level) const = 0;

    // Lowers AST into an IR graph
    virtual void lower(Context &context) const = 0;
};

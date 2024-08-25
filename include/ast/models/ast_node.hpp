#pragma once

#include <string>
#include <iostream>

#include <ast/utils/ast_context.hpp>
#include <ast/models/traits/ast_has_print.hpp>

/**
 *  Base class for nodes in the AST. Only contains virtual functions.
 *  This is the standard interface for all nodes and only contains the functions
 *  that all nodes must implement.
 */
namespace ast
{
    class Node : public HasPrint
    {
    public:
        virtual ~Node() = default;

        // Lowers AST into an IR graph
        virtual void lower(Context &context) const = 0;
    };
}

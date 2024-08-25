#pragma once

#include <iostream>

#include <ast/utils/ast_utils.hpp>

/**
 *  Implementable trait for nodes.
 */
namespace ast
{
    class HasPrint
    {
    public:
        virtual ~HasPrint() = default;

        // Tell and expression to print itself to the given stream
        virtual void print(std::ostream &dst, int indent_level) const = 0;
    };
}

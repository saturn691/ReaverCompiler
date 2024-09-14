#pragma once

/**
 * Types in the AST. Includes basic types, struct/union (definition and usage)
 * and enum (definition and usage).
 */

#include <ast/models/ast_node.hpp>

#include <ir/models/ir_type.hpp>

#include <ty/ty.hpp>

namespace ast
{
    /**
     * Base class for all types in the AST.
     */
    class Type : public Node
    {
    public:
        virtual ir::Type lower() const = 0;
    };

    /**
     * A basic type, such as `int`, `char`, etc.
     */
    class BasicType : public Type
    {
    public:
        BasicType(const ty::Types type);

        ir::Type lower() const override;

        void print(std::ostream &dst, int indent_level) const override;

    private:
        ty::Types type;
    };
}

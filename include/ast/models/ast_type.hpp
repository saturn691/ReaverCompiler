#pragma once

/**
 * Types in the AST. Includes basic types, struct/union (definition and usage)
 * and enum (definition and usage).
 */

#include <ast/models/ast_node.hpp>

#include <ir/models/ir_type.hpp>

namespace ast
{
    /**
     *  Types available in C/C++.
     *  Note that unsigned floats, doubles and long doubles do not exist.
     *  Sorted by priority. Higher value = more priority.
     */
    enum class Types
    {
        VOID,
        UNSIGNED_CHAR,
        CHAR,
        UNSIGNED_SHORT,
        SHORT,
        UNSIGNED_INT,
        INT,
        UNSIGNED_LONG,
        LONG,
        FLOAT,
        DOUBLE,
        LONG_DOUBLE
    };

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
        BasicType(const Types type);

        ir::Type lower() const override;

        static ir::Type to_ir_type(Types type);

        void print(std::ostream &dst, int indent_level) const override;

    private:
        Types type;
    };
}

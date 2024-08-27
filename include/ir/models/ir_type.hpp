#pragma once

#include <ir/models/traits/ir_has_print.hpp>

namespace ir
{
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

    class Type : public HasPrint
    {
    public:
        Type() = default;

        Type(const Types type);

        void print(std::ostream &dst, int indent_level) const override;

    private:
        Types type;
    };
}

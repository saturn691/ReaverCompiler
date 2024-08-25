#pragma once

#include <ir/models/traits/ir_has_print.hpp>

namespace ir
{
    /**
     * Base class for all statements in the IR
     */
    class Statement : public HasPrint
    {
    public:
        virtual ~Statement() = default;

        virtual void print(std::ostream &dst, int indent_level) const override = 0;
    };
}

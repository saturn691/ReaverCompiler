#pragma once

#include <ir/models/traits/ir_has_print.hpp>

namespace ir
{
    /**
     * Base class for all terminators in the IR
     */
    class Terminator : public HasPrint
    {
    public:
        virtual ~Terminator() = default;

        virtual void print(std::ostream &dst, int indent_level) const override = 0;
    };
}

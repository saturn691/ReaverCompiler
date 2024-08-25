#pragma once

#include <ir/models/traits/ir_has_print.hpp>

namespace ir
{
    class Rvalue : public HasPrint
    {
    public:
        virtual void print(std::ostream &dst, int indent_level) const override = 0;
    };
}

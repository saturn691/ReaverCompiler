#pragma once

#include <ir/models/traits/ir_has_print.hpp>

namespace ir
{
    class Type : public HasPrint
    {
    public:
        Type() = default;

        void print(std::ostream &dst, int indent_level) const override;
    };
}

#pragma once

#include <ir/models/ir_has_print.hpp>

#include <ty/ty.hpp>

namespace ir
{
    class Type : public HasPrint
    {
    public:
        Type(const ty::Types type);

        void print(std::ostream &dst, int indent_level) const override;

        const ty::Types type;
    };
}

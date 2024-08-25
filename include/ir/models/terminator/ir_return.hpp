#pragma once

#include <ir/models/terminator/ir_terminator.hpp>

namespace ir
{
    class Return : public Terminator
    {
    public:
        Return();

        void print(std::ostream &dst, int indent_level) const override;
    };
}

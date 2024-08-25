#pragma once

#include <ir/models/terminator/ir_terminator.hpp>
#include <ir/models/ir_basic_block.hpp>

namespace ir
{
    class Goto : public Terminator
    {
    public:
        Goto(const BasicBlock &bb);

        void print(std::ostream &dst, int indent_level) const override;

    private:
        const BasicBlock &bb;
    };
}

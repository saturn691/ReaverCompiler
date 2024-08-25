#pragma once

#include <vector>

#include <ir/models/terminator/ir_terminator.hpp>
#include <ir/models/ir_basic_block.hpp>

namespace ir
{
    class SwitchInt : public Terminator
    {
    public:
        SwitchInt(const int value, const std::vector<BasicBlock &> &bbs);

        void print(std::ostream &dst, int indent_level) const override;

    private:
        // The value to switch on
        const int value;
        // The targets for each case
        const std::vector<BasicBlock &> &bbs;
    };
}

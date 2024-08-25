#include <ir/models/terminator/ir_switch_int.hpp>

namespace ir
{
    SwitchInt::SwitchInt(
        const int value,
        const std::vector<BasicBlock &> &bbs)
        : value(value),
          bbs(bbs)
    {
    }

    void SwitchInt::print(std::ostream &dst, int indent_level) const
    {
    }
}

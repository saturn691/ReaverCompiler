#include <ir/models/terminator/ir_goto.hpp>

namespace ir
{
    Goto::Goto(const BasicBlock &bb)
        : bb(bb)
    {
    }

    void Goto::print(std::ostream &dst, int indent_level) const
    {
    }
}

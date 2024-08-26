#include <ir/models/terminator/ir_call.hpp>

namespace ir
{
    Call::Call(
        const std::string &func,
        const std::vector<std::string> &args,
        const Declaration &ret,
        const BasicBlock &bb)
        : func(func),
          args(args),
          ret(ret),
          bb(bb)
    {
    }

    void Call::print(std::ostream &dst, int indent_level) const
    {
    }
}

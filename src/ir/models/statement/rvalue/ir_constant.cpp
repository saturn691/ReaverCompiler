#include <ir/models/statement/rvalue/ir_constant.hpp>

namespace ir
{
    Constant::Constant(const std::string &value)
        : value(value)
    {
    }

    void Constant::print(std::ostream &dst, int indent_level) const
    {
    }
}

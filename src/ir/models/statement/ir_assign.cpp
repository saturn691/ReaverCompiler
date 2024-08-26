#include <ir/models/statement/ir_assign.hpp>

namespace ir
{
    Assign::Assign(
        const Declaration &lhs,
        const Rvalue &rhs)
        : lhs(lhs), rhs(rhs)
    {
    }

    void Assign::print(std::ostream &dst, int indent_level) const
    {
    }
}

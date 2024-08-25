#include <ir/models/statement/ir_assign.hpp>

namespace ir
{
    Assign::Assign(
        const LocalDeclaration &lhs,
        const Rvalue &rhs)
        : lhs(lhs), rhs(rhs)
    {
    }

    void Assign::print(std::ostream &dst, int indent_level) const
    {
    }
}

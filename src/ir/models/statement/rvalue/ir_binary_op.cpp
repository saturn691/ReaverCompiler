#include <ir/models/statement/rvalue/ir_binary_op.hpp>

namespace ir
{
    BinaryOp::BinaryOp(
        const std::variant<Constant, LocalDeclaration> &lhs,
        const std::string &op,
        const std::variant<Constant, LocalDeclaration> &rhs)
        : lhs(lhs),
          op(op),
          rhs(rhs)
    {
    }

    void BinaryOp::print(std::ostream &dst, int indent_level) const
    {
    }
}

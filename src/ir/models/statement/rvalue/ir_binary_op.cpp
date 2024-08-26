#include <ir/models/statement/rvalue/ir_binary_op.hpp>

namespace ir
{
    BinaryOp::BinaryOp(
        const std::variant<Constant, Declaration> &lhs,
        const std::string &op,
        const std::variant<Constant, Declaration> &rhs)
        : lhs(lhs),
          op(op),
          rhs(rhs)
    {
    }

    void BinaryOp::print(std::ostream &dst, int indent_level) const
    {
    }
}

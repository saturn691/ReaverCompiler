#include <ir/models/statement/rvalue/ir_unary_op.hpp>

namespace ir
{
    UnaryOp::UnaryOp(
        const std::string &op,
        const std::variant<Constant, Declaration> &lhs)
        : op(op),
          lhs(lhs)
    {
    }

    void UnaryOp::print(std::ostream &dst, int indent_level) const
    {
    }
}

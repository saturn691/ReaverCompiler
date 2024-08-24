#include <ast/models/operators/ast_binary_op.hpp>

BinaryOp::BinaryOp(
    const Node *left,
    const Node *right,
    const BinaryOpType op)
    : left(std::unique_ptr<const Node>(left)),
      right(std::unique_ptr<const Node>(right)),
      op(op)
{
}

void BinaryOp::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent;
    left->print(dst, 0);
    switch (op)
    {
    case BinaryOpType::ADD:
        dst << " + ";
        break;
    case BinaryOpType::SUB:
        dst << " - ";
        break;
    case BinaryOpType::MUL:
        dst << " * ";
        break;
    case BinaryOpType::DIV:
        dst << " / ";
        break;
    case BinaryOpType::MOD:
        dst << " % ";
        break;
    case BinaryOpType::BITWISE_AND:
        dst << " & ";
        break;
    case BinaryOpType::BITWISE_OR:
        dst << " | ";
        break;
    case BinaryOpType::BITWISE_XOR:
        dst << " ^ ";
        break;
    case BinaryOpType::LSL:
        dst << " << ";
        break;
    case BinaryOpType::LSR:
        dst << " >> ";
        break;
    case BinaryOpType::EQ:
        dst << " == ";
        break;
    case BinaryOpType::NE:
        dst << " != ";
        break;
    case BinaryOpType::LT:
        dst << " < ";
        break;
    case BinaryOpType::GT:
        dst << " > ";
        break;
    case BinaryOpType::LE:
        dst << " <= ";
        break;
    case BinaryOpType::GE:
        dst << " >= ";
        break;
    case BinaryOpType::LOGICAL_AND:
        dst << " && ";
        break;
    case BinaryOpType::LOGICAL_OR:
        dst << " || ";
        break;
    }
    right->print(dst, 0);
}

void BinaryOp::lower(Context &context) const
{
}

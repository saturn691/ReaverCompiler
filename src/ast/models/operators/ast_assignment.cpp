#include <ast/models/operators/ast_assignment.hpp>

namespace ast
{
    Assignment::Assignment(
        const Node *left,
        const Node *right,
        const AssignmentType op)
        : left(std::unique_ptr<const Node>(left)),
          right(std::unique_ptr<const Node>(right)),
          op(op)
    {
    }

    void Assignment::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        dst << indent;
        left->print(dst, 0);
        switch (op)
        {
        case AssignmentType::ASSIGN:
            dst << " = ";
            break;
        case AssignmentType::MUL_ASSIGN:
            dst << " *= ";
            break;
        case AssignmentType::DIV_ASSIGN:
            dst << " /= ";
            break;
        case AssignmentType::MOD_ASSIGN:
            dst << " %= ";
            break;
        case AssignmentType::ADD_ASSIGN:
            dst << " += ";
            break;
        case AssignmentType::SUB_ASSIGN:
            dst << " -= ";
            break;
        case AssignmentType::LEFT_ASSIGN:
            dst << " <<= ";
            break;
        case AssignmentType::RIGHT_ASSIGN:
            dst << " >>= ";
            break;
        case AssignmentType::AND_ASSIGN:
            dst << " &= ";
            break;
        case AssignmentType::XOR_ASSIGN:
            dst << " ^= ";
            break;
        case AssignmentType::OR_ASSIGN:
            dst << " |= ";
            break;
        }
        right->print(dst, 0);
    }

    void Assignment::lower(Context &context) const
    {
    }
}

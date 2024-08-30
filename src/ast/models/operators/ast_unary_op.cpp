#include <ast/models/operators/ast_unary_op.hpp>

namespace ast
{
    UnaryOp::UnaryOp(
        const UnaryOpType op,
        const Node *expr)
        : op(op),
          expr(std::shared_ptr<const Node>(expr))
    {
    }

    void UnaryOp::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        dst << indent;
        switch (op)
        {
        case UnaryOpType::ADDRESS_OF:
            dst << "&";
            break;
        case UnaryOpType::DEREFERENCE:
            dst << "*";
            break;
        case UnaryOpType::PLUS:
            dst << "+";
            break;
        case UnaryOpType::MINUS:
            dst << "-";
            break;
        case UnaryOpType::BITWISE_NOT:
            dst << "~";
            break;
        case UnaryOpType::LOGICAL_NOT:
            dst << "!";
            break;
        case UnaryOpType::PRE_INC:
            dst << "++";
            break;
        case UnaryOpType::PRE_DEC:
            dst << "--";
            break;
        case UnaryOpType::POST_INC:
            expr->print(dst, 0);
            dst << "++";
            return;
        case UnaryOpType::POST_DEC:
            expr->print(dst, 0);
            dst << "--";
            return;
        }
        expr->print(dst, 0);
    }

    void UnaryOp::lower(Context &context) const
    {
    }
}

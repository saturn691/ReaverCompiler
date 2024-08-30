#include <ast/models/ast_expression.hpp>
#include <ast/utils/ast_utils.hpp>

namespace ast
{
    /*************************************************************************
     * BinaryOp implementation
     ************************************************************************/

    BinaryOp::BinaryOp(
        const Expression *left,
        const Expression *right,
        const BinaryOpType op)
        : left(std::shared_ptr<const Expression>(left)),
          right(std::shared_ptr<const Expression>(right)),
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

    /*************************************************************************
     * Identifier implementation
     ************************************************************************/

    Identifier::Identifier(std::string id)
        : id(id)
    {
    }

    void Identifier::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        dst << indent << id;
    }

    std::string Identifier::get_id() const
    {
        return id;
    }
}

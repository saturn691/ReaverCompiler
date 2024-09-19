#include "ir/models/ir_statement.hpp"
#include <ast/models/ast_expression.hpp>
#include <ast/utils/ast_utils.hpp>

namespace ast
{
/*************************************************************************
 * Assignment implementation
 ************************************************************************/

Assignment::Assignment(const Expression *lhs,
    const AssignmentType op,
    const Expression *rhs)
    : lhs(std::unique_ptr<const Expression>(lhs)), op(op),
      rhs(std::unique_ptr<const Expression>(rhs))
{
}

void Assignment::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent;
    lhs->print(dst, 0);
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
    rhs->print(dst, 0);
}

Types_t Assignment::get_type(Context &context) const
{
    return lhs->get_type(context);
}

ExprLowerR_t Assignment::lower(Context &context,
    const std::unique_ptr<ir::BasicBlock> &block,
    const std::optional<ir::Lvalue> &dest) const
{
    // dest CAN be std::nullopt if we are discarding the value
    auto l = *lhs->lower(context);
    rhs->lower(context, block, ir::Lvalue(l));
    // TODO: static cast required here
    return std::make_unique<ir::Use>(l.decl);
}

ExprLowerL_t Assignment::lower(Context &context) const
{
    // Not a valid lvalue
    return nullptr;
}

/*************************************************************************
 * BinaryOp implementation
 ************************************************************************/

BinaryOp::BinaryOp(const Expression *left,
    const Expression *right,
    const BinaryOpType op)
    : left(std::unique_ptr<const Expression>(left)),
      right(std::unique_ptr<const Expression>(right)), op(op)
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

ir::BinaryOpType BinaryOp::to_ir_type(BinaryOpType op)
{
    switch (op)
    {
    case BinaryOpType::ADD:
        return ir::BinaryOpType::ADD;
    case BinaryOpType::SUB:
        return ir::BinaryOpType::SUB;
    case BinaryOpType::MUL:
        return ir::BinaryOpType::MUL;
    case BinaryOpType::DIV:
        return ir::BinaryOpType::DIV;
    case BinaryOpType::MOD:
        return ir::BinaryOpType::MOD;
    case BinaryOpType::BITWISE_AND:
        return ir::BinaryOpType::BITWISE_AND;
    case BinaryOpType::BITWISE_OR:
        return ir::BinaryOpType::BITWISE_OR;
    case BinaryOpType::BITWISE_XOR:
        return ir::BinaryOpType::BITWISE_XOR;
    case BinaryOpType::LSL:
        return ir::BinaryOpType::LSL;
    case BinaryOpType::LSR:
        return ir::BinaryOpType::LSR;
    case BinaryOpType::EQ:
        return ir::BinaryOpType::EQ;
    case BinaryOpType::NE:
        return ir::BinaryOpType::NE;
    case BinaryOpType::LT:
        return ir::BinaryOpType::LT;
    case BinaryOpType::GT:
        return ir::BinaryOpType::GT;
    case BinaryOpType::LE:
        return ir::BinaryOpType::LE;
    case BinaryOpType::GE:
        return ir::BinaryOpType::GE;
    case BinaryOpType::LOGICAL_AND:
    case BinaryOpType::LOGICAL_OR:
        throw std::runtime_error("Invalid operation in BinaryOp::to_ir_type()");
    }
}

Types_t BinaryOp::get_type(Context &context) const
{
    switch (op)
    {
    case BinaryOpType::ADD:
    case BinaryOpType::SUB:
    case BinaryOpType::MUL:
    case BinaryOpType::DIV:
    case BinaryOpType::MOD:
    case BinaryOpType::BITWISE_AND:
    case BinaryOpType::BITWISE_OR:
    case BinaryOpType::BITWISE_XOR:
    case BinaryOpType::LSL:
    case BinaryOpType::LSR:
    {
        Types_t lhs = left->get_type(context);
        Types_t rhs = right->get_type(context);
        return ty::promote(lhs, rhs);
    }

    case BinaryOpType::EQ:
    case BinaryOpType::NE:
    case BinaryOpType::LT:
    case BinaryOpType::GT:
    case BinaryOpType::LE:
    case BinaryOpType::GE:
    case BinaryOpType::LOGICAL_AND:
    case BinaryOpType::LOGICAL_OR:
        return ty::Types::_BOOL;
    }
}

ExprLowerR_t BinaryOp::lower(Context &context,
    const std::unique_ptr<ir::BasicBlock> &block,
    const std::optional<ir::Lvalue> &dest) const
{
    // dest != std::nullopt here (wouldn't make sense)
    ty::Types ty =
        ty::promote(left->get_type(context), right->get_type(context));

    // Cast into NON reference
    ir::Declaration tmp_l = context.get_temp_decl(ty);
    ir::Declaration tmp_r = context.get_temp_decl(ty);

    auto l = left->lower(context, block, ir::Lvalue(tmp_l));
    auto r = right->lower(context, block, ir::Lvalue(tmp_r));
    auto res =
        std::make_unique<ir::BinaryOp>(std::make_unique<const ir::Use>(tmp_l),
            to_ir_type(op),
            std::make_unique<const ir::Use>(tmp_r));

    block->statements.push_back(
        std::make_unique<ir::Assign>(dest.value(), std::move(res)));

    return res;
}

ExprLowerL_t BinaryOp::lower(Context &context) const
{
    // Not a valid lvalue
    return nullptr;
}

/*************************************************************************
 * Constant implementation
 ************************************************************************/

Constant::Constant(const std::string value) : value(value)
{
}

void Constant::print(std::ostream &dst, [[maybe_unused]] int indent_level) const
{

    dst << value;
}

Types_t Constant::get_type(Context &context) const
{
    char suffix = value.back();
    if (suffix == 'u' || suffix == 'U')
    {
        return ty::Types::UNSIGNED_INT;
    }
    else if (suffix == 'l' || suffix == 'L')
    {
        return ty::Types::LONG;
    }
    else if (suffix == 'f' || suffix == 'F')
    {
        return ty::Types::FLOAT;
    }

    /*
        We need more information. If there is a decimal point, it is a
        double. Why?

        Section 6.4.4.2, paragraph 2:
        "A floating constant has a significant part, which may be followed
        by an exponent part and a suffix that specifies its type."

        Section 6.4.4.2, paragraph 4:
        "An unsuffixed floating constant has type double. If suffixed by the
        letter f or F, it has type float. If suffixed by the letter l or L,
        it has type long double."
    */
    if (value.find('.') != std::string::npos ||
        value.find('e') != std::string::npos ||
        value.find('E') != std::string::npos)
    {
        return ty::Types::DOUBLE;
    }

    // Make an educated guess
    return ty::Types::INT;
}

ExprLowerR_t Constant::lower(Context &context,
    const std::unique_ptr<ir::BasicBlock> &block,
    const std::optional<ir::Lvalue> &dest) const
{
    ty::Types ty =
        (dest == std::nullopt) ? get_type(context) : dest->decl.type.type;
    block->statements.push_back(std::make_unique<ir::Assign>(dest.value(),
        std::make_unique<const ir::Constant>(value, ty)));

    return std::make_unique<const ir::Constant>(value, ty);
}

ExprLowerL_t Constant::lower(Context &context) const
{
    // Not a valid lvalue
    return nullptr;
}

/*************************************************************************
 * Identifier implementation
 ************************************************************************/

Identifier::Identifier(const std::string &id) : id(id)
{
}

void Identifier::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent << id;
}

Types_t Identifier::get_type(Context &context) const
{
    return context.get_type(id);
}

std::string Identifier::get_id() const
{
    return id;
}

ExprLowerR_t Identifier::lower(Context &context,
    const std::unique_ptr<ir::BasicBlock> &block,
    const std::optional<ir::Lvalue> &dest) const
{
    // dest != std::nullopt here (wouldn't make sense)
    for (const auto &decl : context.decls)
    {
        if (decl.name == id)
        {
            auto assign = std::make_unique<ir::Assign>(
                dest.value(), std::make_unique<ir::Use>(decl));
            block->statements.push_back(std::move(assign));

            return std::make_unique<const ir::Use>(decl);
        }
    }

    return nullptr;
}

ExprLowerL_t Identifier::lower(Context &context) const
{
    for (const auto &decl : context.decls)
    {
        if (decl.name == id)
        {
            return std::make_unique<ir::Lvalue>(decl);
        }
    }

    return nullptr;
}
} // namespace ast

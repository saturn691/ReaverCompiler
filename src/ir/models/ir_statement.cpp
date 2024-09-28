#include <ir/ir_visitor.hpp>
#include <ir/models/ir_statement.hpp>

namespace ir
{
/*************************************************************************
 * Lvalue implementation
 ************************************************************************/

Lvalue::Lvalue(const Declaration decl) : decl(decl)
{
}

void Lvalue::print(std::ostream &dst, int indent_level) const
{
    decl.print(dst, 0);
}

llvm::Value *Lvalue::accept(Visitor &visitor) const
{
    return visitor.codegen(*this);
}

/*************************************************************************
 * Call implementation
 ************************************************************************/

Call::Call(
    std::unique_ptr<const Rvalue> func,
    std::vector<std::unique_ptr<const Rvalue>> args)
    : func(std::move(func)), args(std::move(args))
{
}

void Call::print(std::ostream &dst, int indent_level) const
{
    dst << "call ";
    func->print(dst, 0);
    dst << "with args";
    for (auto& arg : args)
    {
        arg->print(dst, 0);
        dst << ", ";
    }
}

llvm::Value *Call::accept(Visitor &visitor) const
{
    return visitor.codegen(*this);
}

/*************************************************************************
 * Constant implementation
 ************************************************************************/

Constant::Constant(const std::string &value, ty::Types type)
    : value(value), type(type)
{
}

void Constant::print(std::ostream &dst, int indent_level) const
{
    dst << value;
}

llvm::Value *Constant::accept(Visitor &visitor) const
{
    return visitor.codegen(*this);
}

/*************************************************************************
 * BinaryOp implementation
 ************************************************************************/

BinaryOp::BinaryOp(
    std::unique_ptr<const Rvalue> lhs,
    const BinaryOpType &op,
    std::unique_ptr<const Rvalue> rhs)
    : lhs(std::move(lhs)), op(op), rhs(std::move(rhs))
{
}

void BinaryOp::print(std::ostream &dst, int indent_level) const
{
    std::string indent = get_indent(indent_level);
    dst << indent;
    lhs->print(dst, 0);
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
    }
    rhs->print(dst, 0);
}

llvm::Value *BinaryOp::accept(Visitor &visitor) const
{
    return visitor.codegen(*this);
}

/*************************************************************************
 * Assign implementation
 ************************************************************************/

Assign::Assign(ir::Lvalue lhs, std::unique_ptr<const Rvalue> rhs)
    : lhs(lhs), rhs(std::move(rhs))
{
}

void Assign::print(std::ostream &dst, int indent_level) const
{
    std::string indent = get_indent(indent_level);
    dst << indent;
    lhs.print(dst, 0);
    dst << " = ";
    rhs->print(dst, 0);
    dst << ";\n";
}

llvm::Value *Assign::accept(Visitor &visitor) const
{
    return visitor.codegen(*this);
}

/*************************************************************************
 * Use implementation
 ************************************************************************/

Use::Use(Declaration decl) : decl(decl)
{
}

void Use::print(std::ostream &dst, int indent_level) const
{
    std::string indent = get_indent(indent_level);
    dst << indent;
    decl.print(dst, 0);
}

llvm::Value *Use::accept(Visitor &visitor) const
{
    return visitor.codegen(*this);
}

/*************************************************************************
 * Cast implementation
 ************************************************************************/

Cast::Cast(Type type, Use use) : type(type), val(use)
{
}

void Cast::print(std::ostream &dst, int indent_level) const
{
    std::string indent = get_indent(indent_level);
    dst << indent << "(";
    type.print(dst, 0);
    dst << ") ";
    val.print(dst, 0);
}

llvm::Value *Cast::accept(Visitor &visitor) const
{
    return visitor.codegen(*this);
}
} // namespace ir

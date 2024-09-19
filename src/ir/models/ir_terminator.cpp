#include <ir/ir_visitor.hpp>
#include <ir/models/ir_terminator.hpp>

namespace ir
{
/*************************************************************************
 * Goto implementation
 ************************************************************************/

Goto::Goto(int bb) : bb(bb)
{
}

void Goto::print(std::ostream &dst, int indent_level) const
{
    std::string indent = get_indent(indent_level);
    dst << indent << "goto " << bb << std::endl;
}

void Goto::accept(Visitor &visitor) const
{
    return visitor.codegen(*this);
}

/*************************************************************************
 * SwitchInt implementation
 ************************************************************************/

SwitchInt::SwitchInt(std::unique_ptr<const Rvalue> expr,
    std::map<int, int> vals_to_bb)
    : expr(std::move(expr)), vals_to_bb(vals_to_bb)
{
}

void SwitchInt::print(std::ostream &dst, int indent_level) const
{
    std::string indent = get_indent(indent_level);
    dst << indent << "switch ";
    expr->print(dst, 0);
    dst << " : [";
    bool first = true;
    for (const auto &p : vals_to_bb)
    {
        if (!first)
            dst << ",";
        dst << p.first << ": BasicBlock " << p.second;
        first = false;
    }
    dst << "]" << std::endl;
}

void SwitchInt::accept(Visitor &visitor) const
{
    return visitor.codegen(*this);
}

/*************************************************************************
 * Return implementation
 ************************************************************************/

void Return::print(std::ostream &dst, int indent_level) const
{
    std::string indent = get_indent(indent_level);
    dst << indent << "return" << std::endl;
}

void Return::accept(Visitor &visitor) const
{
    return visitor.codegen(*this);
}

} // namespace ir

#include <ir/models/ir_declaration.hpp>
#include <ir/ir_visitor.hpp>

namespace ir
{
    Declaration::Declaration(
        const std::string &name,
        const Type &type)
        : name(name),
          type(type)
    {
    }

    void Declaration::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = get_indent(indent_level);
        dst << indent;
        type.print(dst, 0);
        if (name != "")
        {
            dst << " " << name;
        }
    }

    std::string Declaration::accept(Visitor &visitor) const
    {
        return visitor.codegen(*this);
    }
}

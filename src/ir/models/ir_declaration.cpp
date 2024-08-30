#include <ir/models/ir_declaration.hpp>

namespace ir
{
    Declaration::Declaration(
        const std::optional<std::string> &name,
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
        if (name.has_value())
        {
            dst << " " << name.value();
        }
    }
}

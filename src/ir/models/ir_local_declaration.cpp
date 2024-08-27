#include <ir/models/ir_local_declaration.hpp>

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
    }
}

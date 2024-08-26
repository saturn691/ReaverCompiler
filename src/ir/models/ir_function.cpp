#include <ir/models/ir_function.hpp>

namespace ir
{
    Function::Function(const std::string name)
        : name(name)
    {
    }

    void Function::print(std::ostream &dst, int indent_level) const
    {
    }
}

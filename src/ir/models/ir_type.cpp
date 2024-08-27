#include <ir/models/ir_type.hpp>

namespace ir
{
    Type::Type(const Types type)
        : type(type)
    {
    }

    void Type::print(std::ostream &dst, int indent_level) const
    {
    }
}

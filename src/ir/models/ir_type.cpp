#include <ir/models/ir_type.hpp>

namespace ir
{
    Type::Type(const ty::Types type)
        : type(type)
    {
    }

    void Type::print(std::ostream &dst, int indent_level) const
    {
        dst << ty::to_string(type);
    }
}

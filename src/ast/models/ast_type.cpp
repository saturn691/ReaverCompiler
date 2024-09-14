#include <ast/models/ast_type.hpp>
#include <ast/utils/ast_utils.hpp>

namespace ast
{
    /*************************************************************************
     * BasicType implementation
     ************************************************************************/

    BasicType::BasicType(const ty::Types type)
        : type(type)
    {
    }

    ir::Type BasicType::lower() const
    {
        return ir::Type(type);
    }

    void BasicType::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        dst << indent << ty::to_string(type);
    }
}

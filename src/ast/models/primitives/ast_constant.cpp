#include <ast/models/primitives/ast_constant.hpp>

Constant::Constant(const std::string value)
    : value(value)
{
}

void Constant::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent << value;
}

void Constant::lower(Context &context) const
{
}

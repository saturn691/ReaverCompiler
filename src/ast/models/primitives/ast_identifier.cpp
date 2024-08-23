#include <ast/models/primitives/ast_identifier.hpp>

Identifier::Identifier(std::string id)
    : id(id)
{
}

void Identifier::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent << id;
}

void Identifier::lower(Context &context) const
{
}

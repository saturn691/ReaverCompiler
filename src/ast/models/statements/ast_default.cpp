#include <ast/models/statements/ast_default.hpp>

Default::Default(
    const Node *statement)
    : Case(nullptr, statement)
{
}

void Default::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent << "default:" << std::endl;
    statement->print(dst, indent_level + 1);
}

void Default::lower(Context &context) const
{
}

#include <ast/models/enum/ast_enum_declaration.hpp>

EnumDeclaration::EnumDeclaration(
    const NodeList *enum_list)
    : enum_list(std::unique_ptr<const NodeList>(enum_list))
{
}

EnumDeclaration::EnumDeclaration(
    const std::string identifier,
    const NodeList *enum_list)
    : identifier(identifier),
      enum_list(std::unique_ptr<const NodeList>(enum_list))
{
}

void EnumDeclaration::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent << "enum " << identifier << std::endl;
    dst << indent << "{" << std::endl;
    enum_list->print(dst, indent_level + 1);
    dst << std::endl;
    dst << indent << "}";
}

void EnumDeclaration::lower(Context &context) const
{
}

unsigned int EnumDeclaration::get_size() const
{
    // sizeof(int)
    return 4;
}

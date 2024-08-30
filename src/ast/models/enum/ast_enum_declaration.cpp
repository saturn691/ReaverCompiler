#include <ast/models/enum/ast_enum_declaration.hpp>

namespace ast
{
    EnumDeclaration::EnumDeclaration(
        const EnumList *enum_list)
        : enum_list(std::shared_ptr<const EnumList>(enum_list))
    {
    }

    EnumDeclaration::EnumDeclaration(
        const std::string identifier,
        const EnumList *enum_list)
        : identifier(identifier),
          enum_list(std::shared_ptr<const EnumList>(enum_list))
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

    ir::Type EnumDeclaration::lower(Context &context) const
    {
    }

    unsigned int EnumDeclaration::get_size() const
    {
        // sizeof(int)
        return 4;
    }
}

#include <ast/models/struct/ast_struct_declaration.hpp>

StructDeclaration::StructDeclaration(
    StructOrUnionType type,
    const NodeList *declarations)
    : type(type),
      declarations(std::unique_ptr<const NodeList>(declarations))
{
}

StructDeclaration::StructDeclaration(
    const StructOrUnionType type,
    const std::string identifier,
    const NodeList *declarations)
    : type(type),
      identifier(identifier),
      declarations(std::unique_ptr<const NodeList>(declarations))
{
}

void StructDeclaration::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent;

    switch (type)
    {
    case StructOrUnionType::STRUCT:
        dst << "struct";
        break;
    case StructOrUnionType::UNION:
        dst << "union";
        break;
    }

    if (!identifier.empty())
    {
        dst << " " << identifier;
    }

    dst << std::endl;
    dst << indent << "{" << std::endl;
    declarations->print(dst, indent_level + 1);
    dst << std::endl;
    dst << indent << "}";
}

void StructDeclaration::lower(Context &context) const
{
}

unsigned int StructDeclaration::get_size() const
{
    return 0;
}

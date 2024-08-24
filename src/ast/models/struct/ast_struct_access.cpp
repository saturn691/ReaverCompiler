#include <ast/models/struct/ast_struct_access.hpp>

StructAccess::StructAccess(
    const Node *expr,
    const StructAccessType access_type,
    const std::string identifier)
    : expr(std::unique_ptr<const Node>(expr)),
      access_type(access_type),
      identifier(identifier)
{
}

void StructAccess::print(std::ostream &dst, int indent_level) const
{
    expr->print(dst, indent_level);

    switch (access_type)
    {
    case StructAccessType::DOT:
        dst << ".";
        break;
    case StructAccessType::ARROW:
        dst << "->";
        break;
    }

    dst << identifier;
}

void StructAccess::lower(Context &context) const
{
}

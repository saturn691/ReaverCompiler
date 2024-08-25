#include <ast/models/enum/ast_enum_item.hpp>

namespace ast
{
    EnumItem::EnumItem(const std::string identifier)
        : identifier(identifier)
    {
    }

    EnumItem::EnumItem(
        const std::string identifier,
        const Node *expr)
        : identifier(identifier),
          expr(expr)
    {
    }

    void EnumItem::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        dst << indent << identifier;
        if (expr)
        {
            dst << " = ";
            expr->print(dst, 0);
        }
    }

    void EnumItem::lower(Context &context) const
    {
    }
}

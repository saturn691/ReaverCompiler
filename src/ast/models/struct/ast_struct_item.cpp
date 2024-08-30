#include <ast/models/struct/ast_struct_item.hpp>

namespace ast
{
    StructItem::StructItem(
        const Type *type,
        const StructItemList *declarators)
        : type(std::shared_ptr<const Type>(type)),
          declarators(std::shared_ptr<const StructItemList>(declarators))
    {
    }

    void StructItem::print(std::ostream &dst, int indent_level) const
    {
        dst << Utils::get_indent(indent_level);
        type->print(dst, 0);
        dst << " ";
        declarators->print(dst, 0);
        dst << ";";
    }

    void StructItem::lower(Context &context) const
    {
    }
}

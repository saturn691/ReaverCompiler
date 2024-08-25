#include <ast/models/struct/ast_struct_item_list.hpp>

namespace ast
{
    void StructItemList::print(std::ostream &dst, int indent_level) const
    {
        print_delim(dst, indent_level, "\n");
    }
}

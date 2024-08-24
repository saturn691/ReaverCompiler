#include <ast/models/declaration/ast_declaration_list.hpp>

void DeclarationList::print(std::ostream &dst, int indent_level) const
{
    print_delim(dst, indent_level, "\n");
}

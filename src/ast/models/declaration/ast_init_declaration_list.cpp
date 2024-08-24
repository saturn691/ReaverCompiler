#include <ast/models/declaration/ast_init_declarator_list.hpp>

void InitDeclaratorList::print(std::ostream &dst, int indent_level) const
{
    print_delim(dst, indent_level, ", ");
}

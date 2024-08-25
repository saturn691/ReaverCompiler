#include <ast/models/declaration/ast_init_declarator_list.hpp>

namespace ast
{
    void InitDeclaratorList::print(std::ostream &dst, int indent_level) const
    {
        print_delim(dst, indent_level, ", ");
    }
}

#include <ast/models/array/ast_array_initializer_list.hpp>

namespace ast
{
    void ArrayInitializerList::print(std::ostream &dst, int indent_level) const
    {
        std::cout << "{";
        print_delim(dst, indent_level, ", ");
        std::cout << "}";
    }
}

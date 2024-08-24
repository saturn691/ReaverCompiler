#include <ast/models/function/ast_function_call_list.hpp>

void FunctionCallList::print(std::ostream &dst, int indent_level) const
{
    print_delim(dst, indent_level, ", ");
}

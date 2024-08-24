#include <ast/models/function/ast_function_param_list.hpp>

void FunctionParamList::print(std::ostream &dst, int indent_level) const
{
    print_delim(dst, indent_level, ", ");
}

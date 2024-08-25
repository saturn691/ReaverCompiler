#include <ast/utils/ast_utils.hpp>

namespace ast
{
    std::string Utils::get_indent(int indent_level)
    {
        return std::string(indent_level * spaces, ' ');
    }
}

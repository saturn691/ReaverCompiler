#include <ast/ast_utils.hpp>

std::string Utils::get_indent(int indent_level)
{
    return std::string(indent_level * spaces, ' ');
}

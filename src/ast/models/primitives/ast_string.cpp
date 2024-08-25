#include <ast/models/primitives/ast_string.hpp>

namespace ast
{
    String::String(const std::string value)
        : value(value)
    {
    }

    void String::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        dst << indent << value;
    }

    void String::lower(Context &context) const
    {
    }
}

#include <ast/models/primitives/ast_char.hpp>

Char::Char(const std::string _string)
{
    if (_string[1] == '\\')
    {
        auto it = escape_sequences.find(_string[2]);
        if (it != escape_sequences.end())
        {
            value = it->second;
        }
        else
        {
            throw std::runtime_error("Invalid escape sequence");
        }
    }
    else if (_string.size() != 3)
    {
        throw std::runtime_error("Char literal must be a single character");
    }
    else
    {
        value = _string[1];
    }
}

void Char::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent << "'" << value << "'";
}

void Char::lower(Context &context) const
{
}

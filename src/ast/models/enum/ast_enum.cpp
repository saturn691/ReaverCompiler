#include <ast/models/enum/ast_enum.hpp>

namespace ast
{
    Enum::Enum(const std::string identifier)
        : identifier(identifier)
    {
    }

    void Enum::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        dst << indent << "enum " << identifier;
    }

    ir::Type Enum::lower(Context &context) const
    {
    }

    unsigned int Enum::get_size() const
    {
        // sizeof(int)
        return 4;
    }
}

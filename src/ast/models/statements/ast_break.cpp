#include <ast/models/statements/ast_break.hpp>

namespace ast
{
    Break::Break()
    {
    }

    void Break::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        dst << indent << "break;";
        // Buffer is flushed in parent function
    }

    void Break::lower(Context &context) const
    {
    }
}

#include <ast/models/ast_scope.hpp>

namespace ast
{
    Scope::Scope()
    {
    }

    Scope::Scope(const NodeList *declarations)
        : declarations(declarations)
    {
    }

    Scope::Scope(
        const NodeList *declarations,
        const NodeList *statements)
        : declarations(declarations),
          statements(statements)
    {
    }

    void Scope::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        dst << indent << "{" << std::endl;
        if (declarations != nullptr)
        {
            declarations->print(dst, indent_level + 1);
        }
        if (statements != nullptr)
        {
            statements->print(dst, indent_level + 1);
        }
        dst << indent << "}";
    }

    void Scope::lower(Context &context) const
    {
    }
}

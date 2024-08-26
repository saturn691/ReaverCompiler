#include <ast/models/primitives/ast_parenthesis.hpp>

namespace ast
{
    Parenthesis::Parenthesis(const Node *expr)
        : expr(std::unique_ptr<const Node>(expr))
    {
    }

    void Parenthesis::print(std::ostream &dst, int indent_level) const
    {
        dst << Utils::get_indent(indent_level);
        dst << "(";
        expr->print(dst, 0);
        dst << ")";
    }

    template <typename... Args>
    decltype(auto) Parenthesis::lower(Context &context, Args &&...args) const
    {
        expr->lower(context);
    }
}

#include <ast/models/statements/ast_return.hpp>

Return::Return()
{
}

Return::Return(const Node *expression)
    : expression(expression)
{
}

void Return::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent << "return ";
    if (expression != nullptr)
    {
        expression->print(dst, 0);
    }
    dst << ";" << std::endl;
}

void Return::lower(Context &context) const
{
}

#include <ast/models/declaration/ast_pointer_declarator.hpp>

PointerDeclarator::PointerDeclarator(
    int amount,
    const Node *declarator)
    : amount(amount),
      declarator(std::unique_ptr<const Node>(declarator))
{
}

void PointerDeclarator::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    std::string pointers(amount, '*');
    dst << indent << pointers;
    declarator->print(dst, 0);
}

void PointerDeclarator::lower(Context &context) const
{
}

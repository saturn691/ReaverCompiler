#include <ast/models/declaration/ast_init_declarator.hpp>

namespace ast
{
    InitDeclarator::InitDeclarator(
        const Node *declarator,
        const Node *initializer)
        : declarator(std::unique_ptr<const Node>(declarator)),
          initializer(std::unique_ptr<const Node>(initializer))
    {
    }

    void InitDeclarator::print(std::ostream &dst, int indent_level) const
    {
        declarator->print(dst, indent_level);
        dst << " = ";
        initializer->print(dst, 0);
    }

    void InitDeclarator::lower(Context &context) const
    {
    }
}

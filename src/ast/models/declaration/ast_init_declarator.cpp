#include <ast/models/declaration/ast_init_declarator.hpp>

namespace ast
{
    InitDeclarator::InitDeclarator(
        const Declarator *declarator,
        const Node *initializer)
        : declarator(std::unique_ptr<const Declarator>(declarator)),
          initializer(std::unique_ptr<const Node>(initializer))
    {
    }

    void InitDeclarator::print(std::ostream &dst, int indent_level) const
    {
        declarator->print(dst, indent_level);
        dst << " = ";
        initializer->print(dst, 0);
    }

    std::vector<ir::Declaration>
    InitDeclarator::lower(Context &context) const
    {
    }

    std::string InitDeclarator::get_id() const
    {
        return declarator->get_id();
    }
}

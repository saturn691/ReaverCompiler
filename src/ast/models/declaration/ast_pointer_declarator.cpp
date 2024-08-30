#include <ast/models/declaration/ast_pointer_declarator.hpp>

namespace ast
{
    PointerDeclarator::PointerDeclarator(
        int amount,
        const Declarator *declarator)
        : amount(amount),
          declarator(std::shared_ptr<const Declarator>(declarator))
    {
    }

    void PointerDeclarator::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        std::string pointers(amount, '*');
        dst << indent << pointers;
        declarator->print(dst, 0);
    }

    std::string PointerDeclarator::get_id() const
    {
        return declarator->get_id();
    }
}

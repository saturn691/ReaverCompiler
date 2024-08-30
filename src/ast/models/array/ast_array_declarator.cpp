#include <ast/models/array/ast_array_declarator.hpp>

namespace ast
{
    ArrayDeclarator::ArrayDeclarator(
        const Declarator *declarator)
        : declarator(std::shared_ptr<const Declarator>(declarator)),
          size(nullptr)
    {
    }

    ArrayDeclarator::ArrayDeclarator(
        const Declarator *declarator,
        const Node *size)
        : declarator(std::shared_ptr<const Declarator>(declarator)),
          size(std::shared_ptr<const Node>(size))
    {
    }

    void ArrayDeclarator::print(std::ostream &dst, int indent_level) const
    {
        declarator->print(dst, indent_level);
        dst << "[";
        size->print(dst, 0);
        dst << "]";
    }

    std::string ArrayDeclarator::get_id() const
    {
        return declarator->get_id();
    }
}

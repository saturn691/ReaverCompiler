#include <ast/models/struct/ast_struct_item_declarator.hpp>

namespace ast
{
    StructItemDeclarator::StructItemDeclarator(
        const Node *declarator)
        : declarator(declarator)
    {
    }

    void StructItemDeclarator::print(std::ostream &dst, int indent_level) const
    {
        declarator->print(dst, indent_level);
    }

    void StructItemDeclarator::lower(Context &context) const
    {
    }
}

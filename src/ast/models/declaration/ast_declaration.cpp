#include <ast/models/declaration/ast_declaration.hpp>

namespace ast
{
    Declaration::Declaration(
        const Type *specifiers)
        : specifiers(std::unique_ptr<const Type>(specifiers)),
          init_list(nullptr)
    {
    }

    Declaration::Declaration(
        const Type *specifiers,
        const DeclaratorList *init_list)
        : specifiers(std::unique_ptr<const Type>(specifiers)),
          init_list(std::unique_ptr<const DeclaratorList>(init_list))
    {
    }

    void Declaration::print(std::ostream &dst, int indent_level) const
    {
        specifiers->print(dst, indent_level);
        if (init_list)
        {
            dst << " ";
            init_list->print(dst, 0);
        }
        dst << ";" << std::endl;
    }

    void Declaration::lower(Context &context, ir::IR &ir) const
    {
        std::cout << "Declaration::lower" << std::endl;
    }
}

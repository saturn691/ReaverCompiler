#include <ast/models/declaration/ast_declaration.hpp>

Declaration::Declaration(
    const Type *specifiers)
    : specifiers(std::unique_ptr<const Type>(specifiers)),
      init_list(nullptr)
{
}

Declaration::Declaration(
    const Type *specifiers,
    const NodeList *init_list)
    : specifiers(std::unique_ptr<const Type>(specifiers)),
      init_list(std::unique_ptr<const NodeList>(init_list))
{
}

void Declaration::print(std::ostream &dst, int indent_level) const
{
    specifiers->print(dst, indent_level);
    dst << " ";
    init_list->print(dst, 0);
    dst << ";" << std::endl;
}

void Declaration::lower(Context &context) const
{
}

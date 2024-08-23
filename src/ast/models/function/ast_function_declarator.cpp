#include <ast/models/function/ast_function_declarator.hpp>

FunctionDeclarator::FunctionDeclarator(
    const Node *declarator)
    : declarator(declarator)
{
}

FunctionDeclarator::FunctionDeclarator(
    const Node *declarator,
    const NodeList *params)
    : declarator(declarator),
      params(params)
{
}

void FunctionDeclarator::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent;
    declarator->print(dst, 0);
    dst << "(";
    dst << ")";
}

void FunctionDeclarator::lower(Context &context) const
{
}

#include <ast/models/function/ast_function_declarator.hpp>

namespace ast
{
    FunctionDeclarator::FunctionDeclarator(
        const Node *declarator)
        : declarator(declarator)
    {
    }

    FunctionDeclarator::FunctionDeclarator(
        const Node *declarator,
        const FunctionParamList *params)
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
        if (params != nullptr)
        {
            params->print(dst, 0);
        }
        dst << ")";
    }

    void FunctionDeclarator::lower(Context &context) const
    {
    }
}

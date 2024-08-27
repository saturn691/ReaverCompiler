#include <ast/models/function/ast_function_declarator.hpp>

namespace ast
{
    FunctionDeclarator::FunctionDeclarator(
        const Declarator *declarator)
        : declarator(std::unique_ptr<const Declarator>(declarator)),
          params(nullptr)
    {
    }

    FunctionDeclarator::FunctionDeclarator(
        const Declarator *declarator,
        const FunctionParamList *params)
        : declarator(std::unique_ptr<const Declarator>(declarator)),
          params(std::unique_ptr<const FunctionParamList>(params))
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

    std::vector<ir::Declaration>
    FunctionDeclarator::lower(Context &context) const
    {
        return params->lower(context);
    }

    std::string FunctionDeclarator::get_id() const
    {
        return declarator->get_id();
    }
}

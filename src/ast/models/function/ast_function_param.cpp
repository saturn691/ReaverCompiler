#include <ast/models/function/ast_function_param.hpp>

#include <optional>

namespace ast
{
    FunctionParam::FunctionParam(const Type *type)
        : type(type),
          identifier(nullptr)
    {
    }

    FunctionParam::FunctionParam(
        const Type *type,
        const Declarator *identifier)
        : type(type),
          identifier(identifier)
    {
    }

    void FunctionParam::print(std::ostream &dst, int indent_level) const
    {
        dst << Utils::get_indent(indent_level);
        type->print(dst, indent_level);
        if (identifier != nullptr)
        {
            dst << " ";
            identifier->print(dst, 0);
        }
    }

    ir::Declaration FunctionParam::lower(Context &context) const
    {
        std::optional<std::string> ident = std::nullopt;
        if (identifier != nullptr)
        {
            ident = identifier->get_id();
        }
        return ir::Declaration(
            ident,
            type->lower(context));
    }
}

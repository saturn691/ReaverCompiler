#include <ast/models/function/ast_function_param.hpp>

FunctionParam::FunctionParam(const Type *type)
    : type(type),
      identifier(nullptr)
{
}

FunctionParam::FunctionParam(
    const Type *type,
    const Node *identifier)
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

void FunctionParam::lower(Context &context) const
{
}

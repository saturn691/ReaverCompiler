#include <ast/models/function/ast_function_call.hpp>

namespace ast
{
    FunctionCall::FunctionCall(
        const Node *declarator,
        const FunctionCallList *args)
        : declarator(declarator),
          args(args)
    {
    }

    void FunctionCall::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        declarator->print(dst, indent_level);
        dst << "(";
        args->print(dst, indent_level);
        dst << ")";
    }

    void FunctionCall::lower(Context &context) const
    {
    }
}

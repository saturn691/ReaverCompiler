#include <ast/models/function/ast_function_definition.hpp>

namespace ast
{
    FunctionDefinition::FunctionDefinition(
        const Type *specifiers,
        const Node *declarator,
        const Node *statement)
        : specifiers(std::unique_ptr<const Type>(specifiers)),
          declarator(std::unique_ptr<const Node>(declarator)),
          statement(std::unique_ptr<const Node>(statement))
    {
    }

    void FunctionDefinition::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        dst << indent;
        specifiers->print(dst, 0);
        dst << " ";
        declarator->print(dst, 0);
        dst << std::endl;
        statement->print(dst, indent_level);
        dst << std::endl;
    }

    void FunctionDefinition::lower(Context &context, ir::IR &ir) const
    {
        std::cout << "FunctionDefinition::lower" << std::endl;
    }
}

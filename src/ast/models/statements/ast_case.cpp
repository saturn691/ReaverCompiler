#include <ast/models/statements/ast_case.hpp>

Case::Case(
    const Node *expression,
    const Node *statement)
    : expression(std::unique_ptr<const Node>(expression)),
      statement(std::unique_ptr<const Node>(statement))
{
}

void Case::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent << "case ";
    expression->print(dst, 0);
    dst << ":" << std::endl;
    statement->print(dst, indent_level + 1);
}

void Case::lower(Context &context) const
{
}

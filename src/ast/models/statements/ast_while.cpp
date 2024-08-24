#include <ast/models/statements/ast_while.hpp>

While::While(
    const Node *condition,
    const Node *statement)
    : condition(std::unique_ptr<const Node>(condition)),
      statement(std::unique_ptr<const Node>(statement))
{
}

void While::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent << "while (";
    condition->print(dst, 0);
    dst << ")" << std::endl;
    statement->print(dst, indent_level);
}

void While::lower(Context &context) const
{
}

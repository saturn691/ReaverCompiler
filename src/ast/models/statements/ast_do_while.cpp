#include <ast/models/statements/ast_do_while.hpp>

DoWhile::DoWhile(
    const Node *statement,
    const Node *condition)
    : statement(std::unique_ptr<const Node>(statement)),
      condition(std::unique_ptr<const Node>(condition))
{
}

void DoWhile::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent << "do ";
    statement->print(dst, indent_level);
    dst << " while (";
    condition->print(dst, 0);
    dst << ");" << std::endl;
}

void DoWhile::lower(Context &context) const
{
}

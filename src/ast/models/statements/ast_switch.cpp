#include <ast/models/statements/ast_switch.hpp>

Switch::Switch(
    const Node *expression,
    const Node *statement)
    : expression(std::unique_ptr<const Node>(expression)),
      statement(std::unique_ptr<const Node>(statement))
{
}

void Switch::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent << "switch (";
    expression->print(dst, 0);
    dst << ")" << std::endl;
    statement->print(dst, indent_level);
}

void Switch::lower(Context &context) const
{
}

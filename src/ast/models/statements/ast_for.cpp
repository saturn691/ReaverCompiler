#include <ast/models/statements/ast_for.hpp>

For::For(
    const Node *init,
    const Node *condition,
    const Node *statement)
    : init(std::unique_ptr<const Node>(init)),
      condition(std::unique_ptr<const Node>(condition)),
      iteration(nullptr),
      statement(std::unique_ptr<const Node>(statement))
{
}

For::For(
    const Node *init,
    const Node *condition,
    const Node *iteration,
    const Node *statement)
    : init(std::unique_ptr<const Node>(init)),
      condition(std::unique_ptr<const Node>(condition)),
      iteration(std::unique_ptr<const Node>(iteration)),
      statement(std::unique_ptr<const Node>(statement))
{
}

void For::print(std::ostream &dst, int indent_level) const
{
    dst << Utils::get_indent(indent_level) << "for (";
    init->print(dst, 0);
    dst << " ";
    condition->print(dst, 0);
    if (iteration)
    {
        dst << " ";
        iteration->print(dst, 0);
    }
    dst << ")" << std::endl;
    statement->print(dst, indent_level);
}

void For::lower(Context &context) const
{
}

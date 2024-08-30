#include <ast/models/statements/ast_if.hpp>

namespace ast
{
    If::If(
        const Node *condition,
        const Node *statement)
        : condition(std::shared_ptr<const Node>(condition)),
          statement(std::shared_ptr<const Node>(statement)),
          else_statement(nullptr)
    {
    }

    If::If(
        const Node *condition,
        const Node *statement,
        const Node *else_statement)
        : condition(std::shared_ptr<const Node>(condition)),
          statement(std::shared_ptr<const Node>(statement)),
          else_statement(std::shared_ptr<const Node>(else_statement))
    {
    }

    void If::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        dst << indent << "if (";
        condition->print(dst, 0);
        dst << ")" << std::endl;

        // Will flush the buffer
        statement->print(dst, indent_level);

        if (else_statement)
        {
            dst << indent << "else" << std::endl;
            else_statement->print(dst, indent_level);
        }
    }

    void If::lower(Context &context) const
    {
    }
}

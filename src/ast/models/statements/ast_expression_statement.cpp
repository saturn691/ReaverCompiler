#include <ast/models/statements/ast_expression_statement.hpp>

namespace ast
{
    ExpressionStatement::ExpressionStatement()
        : expression(nullptr)
    {
    }

    ExpressionStatement::ExpressionStatement(const Node *expression)
        : expression(std::unique_ptr<const Node>(expression))
    {
    }

    void ExpressionStatement::print(std::ostream &dst, int indent_level) const
    {
        if (expression)
        {
            expression->print(dst, indent_level);
        }
        dst << ";";
    }

    template <typename... Args>
    void ExpressionStatement::lower(Context &context, Args &&...args) const
    {
        if (expression)
        {
            expression->lower(context, std::forward<Args>(args)...);
        }
    }
}

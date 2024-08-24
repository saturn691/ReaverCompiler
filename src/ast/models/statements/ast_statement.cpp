#include <ast/models/statements/ast_statement.hpp>
#include <ast/models/statements/ast_case.hpp>

Statement::Statement(const Node *statement)
    : statement(std::unique_ptr<const Node>(statement))
{
}

void Statement::print(std::ostream &dst, int indent_level) const
{
    statement->print(dst, indent_level);

    // For aesthetic purposes in case statements, don't flush
    if (dynamic_cast<const Case *>(statement.get()) == nullptr)
    {
        dst << std::endl;
    }
}

void Statement::lower(Context &context) const
{
    statement->lower(context);
}

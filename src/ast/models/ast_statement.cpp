#include <ast/models/ast_statement.hpp>
#include <ast/models/ast_declaration.hpp>
#include <ast/models/ast_expression.hpp>
#include <ast/utils/ast_utils.hpp>

namespace ast
{
    /*************************************************************************
     * StatementList implementation
     ************************************************************************/

    void StatementList::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        print_delim(dst, indent_level, "\n" + indent);
    }

    /*************************************************************************
     * CompoundStatement implementation
     ************************************************************************/

    CompoundStatement::CompoundStatement(
        const DeclarationList *decls,
        const StatementList *stmts)
        : decls(std::shared_ptr<const DeclarationList>(decls)),
          stmts(std::shared_ptr<const StatementList>(stmts))
    {
    }

    void CompoundStatement::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        dst << indent << "{" << std::endl;
        if (decls != nullptr)
        {
            decls->print(dst, indent_level + 1);
            dst << std::endl;
        }
        if (stmts != nullptr)
        {
            stmts->print(dst, indent_level + 1);
            dst << std::endl;
        }
        dst << indent << "}";
    }

    ir::FunctionLocals
    CompoundStatement::lower(
        Context &context,
        const ir::FunctionHeader &header,
        std::vector<ir::BasicBlock> &bbs) const
    {
        ir::FunctionLocals locals = ir::FunctionLocals();

        if (decls != nullptr)
        {
            locals = decls->lower();
        }
        if (stmts != nullptr)
        {
            stmts->lower(context, header, locals, bbs);
        }

        return locals;
    }

    void CompoundStatement::lower(
        Context &context,
        const ir::FunctionHeader &header,
        const ir::FunctionLocals &locals,
        std::vector<ir::BasicBlock> &bbs) const
    {
    }

    /*************************************************************************
     * ExpressionStatement implementation
     ************************************************************************/

    ExpressionStatement::ExpressionStatement(
        const Expression *expr)
        : expr(std::shared_ptr<const Expression>(expr))
    {
    }

    void ExpressionStatement::print(std::ostream &dst, int indent_level) const
    {
        if (expr)
        {
            expr->print(dst, indent_level);
        }
        dst << ";";
    }

    void ExpressionStatement::lower(
        Context &context,
        const ir::FunctionHeader &header,
        const ir::FunctionLocals &locals,
        std::vector<ir::BasicBlock> &bbs) const
    {
    }

    /*************************************************************************
     * Return implementation
     ************************************************************************/

    Return::Return(const Expression *expr)
        : expr(std::shared_ptr<const Expression>(expr))
    {
    }

    void Return::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        dst << indent << "return ";
        if (expr != nullptr)
        {
            expr->print(dst, 0);
        }
        dst << ";";
    }

    void Return::lower(
        Context &context,
        const ir::FunctionHeader &header,
        const ir::FunctionLocals &locals,
        std::vector<ir::BasicBlock> &bbs) const
    {
    }
}

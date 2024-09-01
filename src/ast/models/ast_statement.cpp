#include <ast/models/ast_statement.hpp>
#include <ast/models/ast_declaration.hpp>
#include <ast/models/ast_expression.hpp>
#include <ast/utils/ast_utils.hpp>

#include <ir/models/ir_terminator.hpp>

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
        : decls(std::unique_ptr<const DeclarationList>(decls)),
          stmts(std::unique_ptr<const StatementList>(stmts))
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

    // Call from FunctionDefinition::lower
    ir::FunctionLocals
    CompoundStatement::lower(
        Context &context,
        const ir::FunctionHeader &header,
        ir::BasicBlocks &bbs) const
    {
        ir::FunctionLocals locals = ir::FunctionLocals();

        if (decls != nullptr)
        {
            locals = decls->lower();
        }
        if (stmts != nullptr)
        {
            bbs.emplace_back(std::make_unique<ir::BasicBlock>());
            context.bb = 0;
            stmts->lower(context, header, locals, bbs);
        }

        return locals;
    }

    void CompoundStatement::lower(
        Context &context,
        const ir::FunctionHeader &header,
        ir::FunctionLocals &locals,
        ir::BasicBlocks &bbs) const
    {
    }

    /*************************************************************************
     * ExpressionStatement implementation
     ************************************************************************/

    ExpressionStatement::ExpressionStatement(
        const Expression *expr)
        : expr(std::unique_ptr<const Expression>(expr))
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
        ir::FunctionLocals &locals,
        ir::BasicBlocks &bbs) const
    {
    }

    /*************************************************************************
     * Return implementation
     ************************************************************************/

    Return::Return(const Expression *expr)
        : expr(std::unique_ptr<const Expression>(expr))
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
        ir::FunctionLocals &locals,
        ir::BasicBlocks &bbs) const
    {
        if (expr != nullptr)
        {
            expr->lower(
                context, header, locals, bbs[context.bb], header.ret);
        }
        bbs[context.bb]->terminator = std::make_unique<ir::Return>();
    }
}

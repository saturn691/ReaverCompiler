#include "ir/models/ir_basic_block.hpp"
#include <ast/models/ast_declaration.hpp>
#include <ast/models/ast_expression.hpp>
#include <ast/models/ast_statement.hpp>
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
        print_delim(dst, indent_level, "\n");
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
    void CompoundStatement::lower(
        Context &context,
        const ir::FunctionHeader &header,
        ir::BasicBlocks &bbs) const
    {
        bbs.emplace_back(std::make_unique<ir::BasicBlock>());
        context.bb = 0;

        if (decls != nullptr)
        {
            // InitDeclarators need access to the first BasicBlock
            // as there could be `int x = 0`
            decls->lower(context, bbs[0]);
        }
        if (stmts != nullptr)
        {
            stmts->lower(context, header, bbs);
        }
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
        ir::BasicBlocks &bbs) const
    {
        expr->lower(context, bbs[context.bb], std::nullopt);
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
        ir::BasicBlocks &bbs) const
    {
        if (expr != nullptr)
        {
            // Implicit conversion may be needed
            auto ty = expr->get_type(context);
            if (header.ret.type.type != ty)
            {
                // Cast into NON reference
                ir::Declaration decl = context.get_temp_decl(ty);

                // Looking for an Rvalue
                expr->lower(
                    context, bbs[context.bb], ir::Lvalue(decl));
                bbs[context.bb]->statements.push_back(
                    std::make_unique<ir::Assign>(
                        ir::Lvalue(header.ret),
                        std::make_unique<ir::Cast>(
                            header.ret.type,
                            ir::Use(decl))));
            }
            else
            {
                expr->lower(
                    context,
                    bbs[context.bb],
                    ir::Lvalue(header.ret));
            }
        }
        bbs[context.bb]->terminator = std::make_unique<ir::Return>();
    }
}

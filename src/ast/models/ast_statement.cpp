#include "ir/models/ir_basic_block.hpp"
#include <ast/models/ast_declaration.hpp>
#include <ast/models/ast_expression.hpp>
#include <ast/models/ast_statement.hpp>
#include <ast/utils/ast_utils.hpp>

#include <ir/models/ir_terminator.hpp>

#include <memory>
#include <ratio>
#include <ty/ty.hpp>

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

CompoundStatement::CompoundStatement(const DeclarationList *decls,
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
    dst << indent << "}" << std::endl;
}

// Call from FunctionDefinition::lower
void CompoundStatement::lower(Context &context,
    const ir::FunctionHeader &header,
    ir::BasicBlocks &bbs) const
{
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

ExpressionStatement::ExpressionStatement(const Expression *expr)
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

void ExpressionStatement::lower(Context &context,
    const ir::FunctionHeader &header,
    ir::BasicBlocks &bbs) const
{
    expr->lower(context, bbs[context.bb], std::nullopt);
}

ExprLowerR_t ExpressionStatement::lower(Context &context,
    const std::unique_ptr<ir::BasicBlock> &block,
    const std::optional<ir::Lvalue> &dest) const
{
    return expr->lower(context, block, dest);
}

/*************************************************************************
 * If implementation
 ************************************************************************/

If::If(const Expression *condition, const Statement *statement)
    : condition(std::unique_ptr<const Expression>(condition)),
      statement(std::unique_ptr<const Statement>(statement))
{
}

If::If(const Expression *condition,
    const Statement *statement,
    const Statement *else_statement)
    : condition(std::unique_ptr<const Expression>(condition)),
      statement(std::unique_ptr<const Statement>(statement)),
      else_statement(std::unique_ptr<const Statement>(else_statement))
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

void If::lower(Context &context,
    const ir::FunctionHeader &header,
    ir::BasicBlocks &bbs) const
{
    ir::Declaration is_true = context.get_temp_decl(ty::Types::_BOOL);
    condition->lower(context, bbs[context.bb], is_true);
    int start_bb = context.bb;

    // TRUE node
    context.create_new_bb(bbs);
    statement->lower(context, header, bbs);

    // The BasicBlock that is being pointed to right now is the exit node
    int statement_bb = context.bb;
    int else_statement_bb;

    // Link the start node to the TRUE node
    std::map<int, int> start_map = {{1, statement_bb}};

    // FALSE node
    if (else_statement)
    {
        context.create_new_bb(bbs);
        else_statement->lower(context, header, bbs);
        else_statement_bb = context.bb;
        start_map[0] = else_statement_bb;
    }

    // END node
    if (bbs[statement_bb]->terminator == nullptr || else_statement == nullptr ||
        bbs[else_statement_bb]->terminator == nullptr)
    {
        int end_bb = context.create_new_bb(bbs);

        // Here's the tricky bit: if the nodes uses a goto/return/etc., we
        // want to keep the terminator, otherwise we send it to the end.
        if (bbs[statement_bb]->terminator == nullptr)
        {
            bbs[statement_bb]->terminator = std::make_unique<ir::Goto>(end_bb);
        }
        if (else_statement == nullptr)
        {
            start_map[0] = end_bb;
        }
        if (else_statement != nullptr &&
            bbs[else_statement_bb]->terminator == nullptr)
        {
            bbs[else_statement_bb]->terminator =
                std::make_unique<ir::Goto>(end_bb);
        }
    }

    // Link the start node in the IR
    bbs[start_bb]->terminator = std::make_unique<ir::SwitchInt>(
        std::make_unique<ir::Use>(is_true), start_map);
}

/*************************************************************************
 * For implementation
 ************************************************************************/

For::For(const ExpressionStatement *init,
    const ExpressionStatement *cond,
    const Statement *stmt)
    : init(std::unique_ptr<const ExpressionStatement>(cond)),
      stmt(std::unique_ptr<const Statement>(stmt))
{
}

For::For(const ExpressionStatement *init,
    const ExpressionStatement *cond,
    const Expression *loop,
    const Statement *stmt)
    : init(std::unique_ptr<const ExpressionStatement>(init)),
      cond(std::unique_ptr<const ExpressionStatement>(cond)),
      loop(std::unique_ptr<const Expression>(loop)),
      stmt(std::unique_ptr<const Statement>(stmt))
{
}

void For::print(std::ostream &dst, int indent_level) const
{
    dst << Utils::get_indent(indent_level) << "for (";
    init->print(dst, 0);
    dst << " ";
    cond->print(dst, 0);
    if (loop)
    {
        dst << " ";
        loop->print(dst, 0);
    }
    dst << ")" << std::endl;
    stmt->print(dst, indent_level);
}

void For::lower(Context &context,
    const ir::FunctionHeader &header,
    ir::BasicBlocks &bbs) const
{
    init->lower(context, header, bbs);

    // Entry node must not have predecessors so create new BasicBlock
    int prev_bb = context.bb;
    int start_bb = context.create_new_bb(bbs);
    bbs[prev_bb]->terminator = std::make_unique<ir::Goto>(start_bb);

    ir::Declaration is_true = context.get_temp_decl(ty::Types::_BOOL);
    cond->lower(context, bbs[context.bb], is_true);

    // TRUE node (we want the ENTRY node)
    int statement_bb = context.create_new_bb(bbs);
    stmt->lower(context, header, bbs);
    if (bbs[statement_bb]->terminator == nullptr)
    {
        loop->lower(context, bbs[context.bb], std::nullopt);
        bbs[statement_bb]->terminator = std::make_unique<ir::Goto>(start_bb);
    }

    // FALSE node
    int end_bb = context.create_new_bb(bbs);

    // Link the start node in the IR
    bbs[start_bb]->terminator =
        std::make_unique<ir::SwitchInt>(std::make_unique<ir::Use>(is_true),
            std::map<int, int>{{0, end_bb}, {1, statement_bb}});
}

/*************************************************************************
 * While implementation
 ************************************************************************/

While::While(const Expression *condition, const Statement *statement)
    : condition(std::unique_ptr<const Expression>(condition)),
      statement(std::unique_ptr<const Statement>(statement))
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

void While::lower(Context &context,
    const ir::FunctionHeader &header,
    ir::BasicBlocks &bbs) const
{
    // Entry node must not have predecessors so create new BasicBlock
    int prev_bb = context.bb;
    int start_bb = context.create_new_bb(bbs);
    bbs[prev_bb]->terminator = std::make_unique<ir::Goto>(start_bb);

    ir::Declaration is_true = context.get_temp_decl(ty::Types::_BOOL);
    condition->lower(context, bbs[context.bb], is_true);

    // TRUE node (we want the ENTRY node)
    int statement_bb = context.create_new_bb(bbs);
    statement->lower(context, header, bbs);
    if (bbs[statement_bb]->terminator == nullptr)
    {
        bbs[statement_bb]->terminator = std::make_unique<ir::Goto>(start_bb);
    }

    // FALSE node
    int end_bb = context.create_new_bb(bbs);

    // Link the start node in the IR
    bbs[start_bb]->terminator =
        std::make_unique<ir::SwitchInt>(std::make_unique<ir::Use>(is_true),
            std::map<int, int>{{0, end_bb}, {1, statement_bb}});
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

void Return::lower(Context &context,
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
            expr->lower(context, bbs[context.bb], ir::Lvalue(decl));
            bbs[context.bb]->statements.push_back(std::make_unique<ir::Assign>(
                ir::Lvalue(header.ret),
                std::make_unique<ir::Cast>(header.ret.type, ir::Use(decl))));
        }
        else
        {
            expr->lower(context, bbs[context.bb], ir::Lvalue(header.ret));
        }
    }
    bbs[context.bb]->terminator = std::make_unique<ir::Return>();
}
} // namespace ast

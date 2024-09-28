#include "ir/models/ir_function.hpp"
#include <ast/models/ast_declaration.hpp>
#include <ast/models/ast_expression.hpp>
#include <ast/utils/ast_utils.hpp>

namespace ast
{

/*************************************************************************
 * FunctionDefinition implementation
 ************************************************************************/

FunctionDefinition::FunctionDefinition(
    const Type *specifiers,
    const FunctionDeclarator *declarator,
    const CompoundStatement *statement)
    : specifiers(std::unique_ptr<const Type>(specifiers)),
      declarator(std::unique_ptr<const FunctionDeclarator>(declarator)),
      statement(std::unique_ptr<const CompoundStatement>(statement))
{
}

void FunctionDefinition::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent;
    specifiers->print(dst, 0);
    dst << " ";
    declarator->print(dst, 0);
    dst << std::endl;
    statement->print(dst, indent_level);
    dst << std::endl;
}

void FunctionDefinition::lower(Context &context, std::unique_ptr<ir::IR> &ir)
    const
{
    ir::Declaration return_type = ir::Declaration("", specifiers->lower());
    std::vector<ir::Declaration> args = declarator->lower();
    std::string name = declarator->get_id();
    ir::FunctionHeader header = ir::FunctionHeader(name, return_type, args);

    // Important step: we copy the parameters into Context, as they act
    // like function locals at this point
    std::vector<ty::CompoundType> arg_types;
    context.clear_args();
    for (const auto &arg : args)
    {
        context.decls.push_back(arg);
        context.set_arg_type(arg.name, arg.type.type);
        arg_types.push_back(arg.type.type);
    }

    // Register the function onto the GLOBAL context.
    context.set_type(
        name, ty::to_function_type(return_type.type.type, arg_types));

    ir::BasicBlocks bbs;
    context.create_new_bb(bbs);
    statement->lower(context, header, bbs);
    ir::FunctionLocals locals = ir::FunctionLocals();
    locals.locals = std::move(context.decls);

    ir->add_function(
        std::make_unique<ir::Function>(header, locals, std::move(bbs)));
}

/*************************************************************************
 * FunctionDeclarator implementation
 ************************************************************************/

FunctionDeclarator::FunctionDeclarator(const Declarator *decl)
    : decl(std::unique_ptr<const Declarator>(decl))
{
}

FunctionDeclarator::FunctionDeclarator(
    const Declarator *decl,
    const FunctionParamList *params)
    : decl(std::unique_ptr<const Declarator>(decl)),
      params(std::unique_ptr<const FunctionParamList>(params))
{
}

void FunctionDeclarator::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent;
    decl->print(dst, 0);
    dst << "(";
    if (params != nullptr)
    {
        params->print(dst, 0);
    }
    dst << ")";
}

std::string FunctionDeclarator::get_id() const
{
    return decl->get_id();
}

std::vector<ir::Declaration> FunctionDeclarator::lower() const
{
    std::vector<ir::Declaration> args;
    if (params != nullptr)
    {
        args = params->lower();
    }
    return args;
}

/*************************************************************************
 * FunctionParamList implementation
 ************************************************************************/

void FunctionParamList::print(std::ostream &dst, int indent_level) const
{
    print_delim(dst, indent_level, ", ");
}

std::vector<ir::Declaration> FunctionParamList::lower() const
{
    std::vector<ir::Declaration> args;
    for (const auto &param : nodes)
    {
        args.push_back(std::get<0>(param)->lower());
    }
    return args;
}

/*************************************************************************
 * FunctionParam implementation
 ************************************************************************/

FunctionParam::FunctionParam(const Type *type)
    : type(std::unique_ptr<const Type>(type))
{
}

FunctionParam::FunctionParam(const Type *type, const Declarator *decl)
    : type(std::unique_ptr<const Type>(type)),
      decl(std::unique_ptr<const Declarator>(decl))
{
}

void FunctionParam::print(std::ostream &dst, [[maybe_unused]] int indent_level)
    const
{
    type->print(dst, 0);
    if (decl != nullptr)
    {
        dst << " ";
        decl->print(dst, 0);
    }
}

ir::Declaration FunctionParam::lower() const
{
    std::string id = "";
    if (decl != nullptr)
    {
        id = decl->get_id();
    }

    return ir::Declaration(id, type->lower());
}

/*************************************************************************
 * InitDeclarator implementation
 ************************************************************************/

InitDeclarator::InitDeclarator(const Declarator *decl)
    : decl(std::unique_ptr<const Declarator>(decl))
{
}

InitDeclarator::InitDeclarator(const Declarator *decl, const Expression *init)
    : decl(std::unique_ptr<const Declarator>(decl)),
      init(std::unique_ptr<const Expression>(init))
{
}

void InitDeclarator::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent;
    decl->print(dst, 0);
    if (init != nullptr)
    {
        dst << " = ";
        init->print(dst, 0);
    }
}

void InitDeclarator::lower(
    Context &context,
    const std::unique_ptr<ir::BasicBlock> &bb,
    const std::unique_ptr<const Type> &ty) const
{
    ir::Declaration &declaration = context.add_declaration(ty, decl);
    if (init)
    {
        init->lower(context, bb, declaration);
    }
}

std::string InitDeclarator::get_id() const
{
    return decl->get_id();
}

/*************************************************************************
 * InitDeclaratorList implementation
 ************************************************************************/

void InitDeclaratorList::lower(
    Context &context,
    const std::unique_ptr<ir::BasicBlock> &bb,
    const std::unique_ptr<const Type> &ty) const
{
    for (const auto &decl : nodes)
    {
        std::get<0>(decl)->lower(context, bb, ty);
    }
}

/*************************************************************************
 * DeclarationNode implementation
 ************************************************************************/

DeclarationNode::DeclarationNode(const Type *specifiers)
    : specifiers(std::unique_ptr<const Type>(specifiers))
{
}

DeclarationNode::DeclarationNode(
    const Type *specifiers,
    const InitDeclaratorList *decls)
    : specifiers(std::unique_ptr<const Type>(specifiers)),
      decls(std::unique_ptr<const InitDeclaratorList>(decls))
{
}

void DeclarationNode::print(std::ostream &dst, int indent_level) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent;
    specifiers->print(dst, 0);
    dst << " ";
    decls->print(dst, 0);
    dst << ";";
}

// Globals
void DeclarationNode::lower(Context &context, std::unique_ptr<ir::IR> &ir) const
{
    // std::vector<ir::Declaration> decls = decls->lower(context);
    // for (const auto &decl : decls)
    // {
    //     ir.add_declaration(decl);
    // }
}

void DeclarationNode::lower(
    Context &context,
    const std::unique_ptr<ir::BasicBlock> &bb) const
{
    if (decls)
    {
        decls->lower(context, bb, specifiers);
    }
    else
    {
        // Struct or union declaration
        /*decls->lower(bb);   */
    }
}

/*************************************************************************
 * DeclarationList implementation
 ************************************************************************/

void DeclarationList::lower(
    Context &context,
    const std::unique_ptr<ir::BasicBlock> &bb) const
{
    for (const auto &decl : nodes)
    {
        std::get<0>(decl)->lower(context, bb);
    }
}

} // namespace ast

#include <ast/models/ast_declaration.hpp>
#include <ast/models/ast_expression.hpp>
#include <ast/utils/ast_utils.hpp>

namespace ast
{
    /*************************************************************************
     * DeclarationList implementation
     ************************************************************************/

    ir::FunctionLocals
    DeclarationList::lower() const
    {
        return ir::FunctionLocals();
    }

    /*************************************************************************
     * FunctionDefinition implementation
     ************************************************************************/

    FunctionDefinition::FunctionDefinition(
        const Type *specifiers,
        const FunctionDeclarator *declarator,
        const CompoundStatement *statement)
        : specifiers(std::shared_ptr<const Type>(specifiers)),
          declarator(std::shared_ptr<const FunctionDeclarator>(declarator)),
          statement(std::shared_ptr<const CompoundStatement>(statement))
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

    void FunctionDefinition::lower(Context &context, ir::IR &ir) const
    {
        ir::Declaration return_type = ir::Declaration(
            std::nullopt,
            specifiers->lower());
        std::vector<ir::Declaration> args = declarator->lower();
        std::string name = declarator->get_id();
        ir::FunctionHeader header = ir::FunctionHeader(name, return_type, args);

        std::vector<ir::BasicBlock> bbs;
        ir::FunctionLocals locals = statement->lower(context, header, bbs);

        ir.add_function(ir::Function(header, locals, std::move(bbs)));
    }

    /*************************************************************************
     * FunctionDeclarator implementation
     ************************************************************************/

    FunctionDeclarator::FunctionDeclarator(
        const Declarator *decl)
        : decl(std::shared_ptr<const Declarator>(decl))
    {
    }

    FunctionDeclarator::FunctionDeclarator(
        const Declarator *decl,
        const FunctionParamList *params)
        : decl(std::shared_ptr<const Declarator>(decl)),
          params(std::shared_ptr<const FunctionParamList>(params))
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
        : type(std::shared_ptr<const Type>(type))
    {
    }

    FunctionParam::FunctionParam(
        const Type *type,
        const Declarator *decl)
        : type(std::shared_ptr<const Type>(type)),
          decl(std::shared_ptr<const Declarator>(decl))
    {
    }

    void FunctionParam::print(std::ostream &dst, int indent_level) const
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
        std::optional<std::string> id = std::nullopt;
        if (decl != nullptr)
        {
            id = decl->get_id();
        }

        return ir::Declaration(id, type->lower());
    }

    /*************************************************************************
     * InitDeclarator implementation
     ************************************************************************/

    InitDeclarator::InitDeclarator(
        const Declarator *decl)
        : decl(std::shared_ptr<const Declarator>(decl))
    {
    }

    InitDeclarator::InitDeclarator(
        const Declarator *decl,
        const Expression *init)
        : decl(std::shared_ptr<const Declarator>(decl)),
          init(std::shared_ptr<const Expression>(init))
    {
    }

    void InitDeclarator::print(std::ostream &dst, int indent_level) const
    {
        decl->print(dst, 0);
        if (init != nullptr)
        {
            dst << " = ";
            init->print(dst, 0);
        }
    }

    std::string InitDeclarator::get_id() const
    {
        return decl->get_id();
    }

    /*************************************************************************
     * DeclarationNode implementation
     ************************************************************************/

    DeclarationNode::DeclarationNode(
        const Type *specifiers)
        : specifiers(std::shared_ptr<const Type>(specifiers))
    {
    }

    DeclarationNode::DeclarationNode(
        const Type *specifiers,
        const InitDeclaratorList *decls)
        : specifiers(std::shared_ptr<const Type>(specifiers)),
          decls(std::shared_ptr<const InitDeclaratorList>(decls))
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

    void DeclarationNode::lower(Context &context, ir::IR &ir) const
    {
        // std::vector<ir::Declaration> decls = decls->lower(context);
        // for (const auto &decl : decls)
        // {
        //     ir.add_declaration(decl);
        // }
    }

}

#include "AST/Decl.hpp"
#include "AST/Expr.hpp"
#include "AST/Type.hpp"

namespace AST
{

ArrayDecl::ArrayDecl(const Decl *decl, const Expr *size)
    : decl_(decl), size_(size)
{
}

std::string ArrayDecl::getID() const
{
    return decl_->getID();
}

DeclNode::DeclNode(const TypeDecl *type) : type_(type)
{
}

DeclNode::DeclNode(const TypeDecl *type, const InitDeclList *decl)
    : type_(type), initDeclList_(decl)
{
}

FnDef::FnDef(
    const TypeDecl *retType,
    const Decl *decl,
    const CompoundStmt *body)
    : retType_(retType), decl_(decl), body_(body)
{
}

std::string FnDef::getID() const
{
    return decl_->getID();
}

InitDecl::InitDecl(const Decl *decl) : decl_(decl)
{
}

InitDecl::InitDecl(const Decl *decl, const Init *init)
    : decl_(decl), init_(init)
{
}

std::string InitDecl::getID() const
{
    return decl_->getID();
}

std::vector<std::string> DeclNode::getIDs() const
{
    std::vector<std::string> ids;
    for (const auto &decl : initDeclList_->nodes_)
    {
        ids.push_back(
            std::visit([](const auto &decl) { return decl->getID(); }, decl));
    }
    return ids;
}

Struct::Struct(Type type, std::string name, const StructMemberList *members)
    : type_(type), name_(std::move(name)), members_(members)
{
}

Struct::Struct(Type type, const StructMemberList *members)
    : type_(type), members_(members)
{
}

Struct::Struct(Type type, std::string name)
    : type_(type), name_(std::move(name))
{
}

} // namespace AST
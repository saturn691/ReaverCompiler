#include "AST/Decl.hpp"
#include "AST/Expr.hpp"
#include "AST/Type.hpp"

namespace AST
{

DeclNode::DeclNode(const TypeNode *type) : type_(type)
{
}

DeclNode::DeclNode(const TypeNode *type, const InitDeclList *decl)
    : type_(type), initDeclList_(decl)
{
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

} // namespace AST
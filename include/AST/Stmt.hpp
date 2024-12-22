#pragma once

#include <optional>

#include "AST/Node.hpp"

namespace AST
{
// Forward declarations
class Expr;

class Stmt : public virtual BaseNode
{
public:
    virtual ~Stmt() = default;
};

class BlockItemList final : public NodeList<DeclNode, Stmt>,
                            public Node<BlockItemList>
{
public:
    using NodeList::NodeList;
};

class CompoundStmt final : public Node<CompoundStmt>, public Stmt
{
public:
    CompoundStmt() = default;
    CompoundStmt(BlockItemList *nodes) : nodes_(nodes)
    {
    }

    Ptr<BlockItemList> nodes_;
};

class Return final : public Node<Return>, public Stmt
{
public:
    Return() = default;
    Return(const Expr *expr) : expr_(expr)
    {
    }

    std::optional<Ptr<Expr>> expr_;
};

} // namespace AST
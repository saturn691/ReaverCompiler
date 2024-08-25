#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>
#include <ast/models/ast_node_list.hpp>

namespace ast
{
    class Scope : public Node
    {
    public:
        Scope();

        Scope(const NodeList *declarations);

        Scope(const NodeList *declarations, const NodeList *statements);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const override;

    private:
        std::unique_ptr<const NodeList> declarations;
        std::unique_ptr<const NodeList> statements;
    };
}

#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>
#include <ast/models/type/ast_type.hpp>

namespace ast
{
    class FunctionDefinition : public Node
    {
    public:
        FunctionDefinition(
            const Type *specifiers,
            const Node *declarator,
            const Node *statement);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const override;

    private:
        std::unique_ptr<const Type> specifiers;
        std::unique_ptr<const Node> declarator;
        std::unique_ptr<const Node> statement;
    };
}

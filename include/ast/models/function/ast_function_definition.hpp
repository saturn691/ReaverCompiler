#pragma once

#include <memory>

#include <ast/models/declaration/ast_declarator.hpp>
#include <ast/models/type/ast_type.hpp>
#include <ast/models/ast_node.hpp>

#include <ir/models/ir_ir.hpp>

namespace ast
{
    class FunctionDefinition : public Node
    {
    public:
        FunctionDefinition(
            const Type *specifiers,
            const Declarator *declarator,
            const Node *statement);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context, ir::IR &ir) const;

    private:
        std::unique_ptr<const Type> specifiers;
        std::unique_ptr<const Declarator> declarator;
        std::unique_ptr<const Node> statement;
    };
}

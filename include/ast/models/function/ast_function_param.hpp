#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>
#include <ast/models/type/ast_type.hpp>
#include <ast/models/declaration/ast_declarator.hpp>

#include <ir/models/ir_local_declaration.hpp>

namespace ast
{
    class FunctionParam : public Node
    {
    public:
        FunctionParam(const Type *type);

        FunctionParam(const Type *type, const Declarator *identifier);

        void print(std::ostream &dst, int indent_level) const override;

        ir::Declaration lower(Context &context) const;

    private:
        std::unique_ptr<const Type> type;
        std::unique_ptr<const Declarator> identifier;
    };
}

#pragma once

#include <ast/models/ast_node.hpp>
#include <ast/models/ast_node_list.hpp>
#include <ast/models/type/ast_type.hpp>

namespace ast
{
    class Declaration : public Node
    {
    public:
        // For struct/union declarations
        Declaration(const Type *specifiers);

        // For variable declarations
        Declaration(const Type *specifiers, const NodeList *init_list);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const override;

    private:
        std::unique_ptr<const Type> specifiers;
        std::unique_ptr<const NodeList> init_list;
    };
}

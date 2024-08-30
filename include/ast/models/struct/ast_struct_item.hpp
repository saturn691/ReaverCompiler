#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>
#include <ast/models/ast_node_list.hpp>
#include <ast/models/type/ast_type.hpp>
#include <ast/models/struct/ast_struct_item_list.hpp>

namespace ast
{
    class StructItem : public Node
    {
    public:
        StructItem(
            const Type *type,
            const StructItemList *declarators);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const;

    private:
        std::shared_ptr<const Type> type;
        std::shared_ptr<const StructItemList> declarators;
    };
}

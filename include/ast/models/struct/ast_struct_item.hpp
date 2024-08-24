#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>
#include <ast/models/ast_node_list.hpp>
#include <ast/models/type/ast_type.hpp>

class StructItem : public Node
{
public:
    StructItem(
        const Type *type,
        const NodeList *declarators);

    void print(std::ostream &dst, int indent_level) const override;

    void lower(Context &context) const override;

private:
    std::unique_ptr<const Type> type;
    std::unique_ptr<const NodeList> declarators;
};

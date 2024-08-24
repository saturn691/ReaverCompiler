#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>
#include <ast/models/type/ast_type.hpp>

class Sizeof : public Node
{
public:
    Sizeof(const Type *type);

    Sizeof(const Node *expression);

    void print(std::ostream &dst, int indent_level) const override;

    void lower(Context &context) const override;

private:
    std::unique_ptr<const Type> type;
    std::unique_ptr<const Node> expression;
};

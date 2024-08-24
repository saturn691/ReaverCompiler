#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>
#include <ast/models/type/ast_type.hpp>

class FunctionParam : public Node
{
public:
    FunctionParam(const Type *type);

    FunctionParam(const Type *type, const Node *identifier);

    virtual void print(std::ostream &dst, int indent_level) const override;

    virtual void lower(Context &context) const override;

private:
    std::unique_ptr<const Type> type;
    std::unique_ptr<const Node> identifier;
};

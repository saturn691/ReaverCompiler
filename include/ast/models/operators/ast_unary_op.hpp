#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

enum class UnaryOpType
{
    ADDRESS_OF,
    DEREFERENCE,
    PLUS,
    MINUS,
    BITWISE_NOT,
    LOGICAL_NOT,
    PRE_INC,
    PRE_DEC,
    POST_INC,
    POST_DEC
};

namespace ast
{
    class UnaryOp : public Node
    {
    public:
        UnaryOp(const UnaryOpType op, const Node *expr);

        virtual void print(std::ostream &dst, int indent_level) const override;

        virtual void lower(Context &context) const;

    private:
        UnaryOpType op;
        std::unique_ptr<const Node> expr;
    };
}

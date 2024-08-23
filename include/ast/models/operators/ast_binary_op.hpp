#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

enum class BinaryOpType
{
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    BITWISE_AND,
    BITWISE_OR,
    BITWISE_XOR,
    LSL,
    LSR,
};

class BinaryOp : public Node
{
public:
    BinaryOp(const Node *left, const Node *right, const BinaryOpType op);

    virtual void print(std::ostream &dst, int indent_level) const override;

    virtual void lower(Context &context) const override;

private:
    std::unique_ptr<const Node> left;
    std::unique_ptr<const Node> right;
    BinaryOpType op;
};

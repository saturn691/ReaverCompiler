#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

enum class AssignmentType
{
    ASSIGN,
    MUL_ASSIGN,
    DIV_ASSIGN,
    MOD_ASSIGN,
    ADD_ASSIGN,
    SUB_ASSIGN,
    LEFT_ASSIGN,
    RIGHT_ASSIGN,
    AND_ASSIGN,
    XOR_ASSIGN,
    OR_ASSIGN
};

namespace ast
{
    class Assignment : public Node
    {
    public:
        Assignment(const Node *left, const Node *right, const AssignmentType op);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const;

    private:
        std::unique_ptr<const Node> left;
        std::unique_ptr<const Node> right;
        AssignmentType op;
    };
}

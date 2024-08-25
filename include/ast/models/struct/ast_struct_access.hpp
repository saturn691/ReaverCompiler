#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>

enum class StructAccessType
{
    DOT,
    ARROW
};

namespace ast
{
    class StructAccess : public Node
    {
    public:
        StructAccess(
            const Node *expr,
            const StructAccessType access_type,
            const std::string identifier);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const override;

    private:
        std::unique_ptr<const Node> expr;
        StructAccessType access_type;
        std::string identifier;
    };
}

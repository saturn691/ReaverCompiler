#pragma once

#include <memory>

#include <ast/models/declaration/ast_declarator.hpp>

namespace ast
{
    class ArrayDeclarator : public Declarator
    {
    public:
        ArrayDeclarator(const Declarator *declarator);

        ArrayDeclarator(const Declarator *declarator, const Node *size);

        void print(std::ostream &dst, int indent_level) const override;

        std::string get_id() const override;

    private:
        std::shared_ptr<const Declarator> declarator;
        std::shared_ptr<const Node> size;
    };
}

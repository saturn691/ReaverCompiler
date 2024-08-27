#pragma once

#include <memory>

#include <ast/models/declaration/ast_declarator.hpp>

namespace ast
{
    class InitDeclarator : public Declarator
    {
    public:
        InitDeclarator(const Declarator *declarator, const Node *initializer);

        void print(std::ostream &dst, int indent_level) const override;

        std::vector<ir::Declaration> lower(Context &context) const override;

        std::string get_id() const override;

    private:
        std::unique_ptr<const Declarator> declarator;
        std::unique_ptr<const Node> initializer;
    };
}

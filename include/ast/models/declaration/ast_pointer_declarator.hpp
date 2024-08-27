#pragma once

#include <memory>
#include <vector>

#include <ast/models/declaration/ast_declarator.hpp>

namespace ast
{
    class PointerDeclarator : public Declarator
    {
    public:
        PointerDeclarator(int amount, const Declarator *declarator);

        void print(std::ostream &dst, int indent_level) const override;

        std::vector<ir::Declaration> lower(Context &context) const override;

        std::string get_id() const override;

    private:
        int amount;
        std::unique_ptr<const Declarator> declarator;
    };
}

#pragma once

#include <string>

#include <ast/models/declaration/ast_declarator.hpp>

namespace ast
{
    class Identifier : public Declarator
    {
    public:
        Identifier(const std::string id);

        void print(std::ostream &dst, int indent_level) const override;

        std::vector<ir::Declaration> lower(Context &context) const override;

        std::string get_id() const override;

    private:
        std::string id;
    };
}

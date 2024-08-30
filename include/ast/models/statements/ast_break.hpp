#pragma once

#include <ast/models/statements/ast_statement.hpp>

namespace ast
{
    class Break : public Statement
    {
    public:
        Break();

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const;
    };
}

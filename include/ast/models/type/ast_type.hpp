#pragma once

#include <ast/models/traits/ast_has_print.hpp>

namespace ast
{
    class Type : public HasPrint
    {
    public:
        virtual unsigned int get_size() const = 0;

    private:
    };
}

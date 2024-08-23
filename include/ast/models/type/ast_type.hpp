#pragma once

#include <ast/models/traits/ast_has_print.hpp>

class Type : public HasPrint
{
public:
    virtual unsigned int get_size() const = 0;

private:
};

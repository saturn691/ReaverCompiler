#pragma once

#include <memory>

#include <ast/models/statements/ast_case.hpp>

class Default : public Case
{
public:
    Default(const Node *statement);

    void print(std::ostream &dst, int indent_level) const override;

    void lower(Context &context) const override;
};

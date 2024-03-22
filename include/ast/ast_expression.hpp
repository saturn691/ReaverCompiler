#ifndef AST_EXPRESSION_HPP
#define AST_EXPRESSION_HPP

#include "ast_node.hpp"
#include "ast_declarator.hpp"

class Expression : public Declarator
{
public:
    Expression() {}

    virtual ~Expression()
    {}

    virtual double evaluate() const
    {
        throw std::runtime_error("evaluate() not implemented");
    }

    virtual Types get_type(Context &context) const = 0;
};

#endif

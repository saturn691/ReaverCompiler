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

    virtual Types get_type() const = 0;
};

#endif

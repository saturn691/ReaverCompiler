#ifndef AST_EXPRESSION_HPP
#define AST_EXPRESSION_HPP

#include "ast_node.hpp"

class Expression : public Node
{
public:
    virtual ~Expression()
    {}

    virtual Types get_type() const = 0;

    virtual void print(std::ostream &dst, int indent_level) const = 0;

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const = 0;
};


#endif

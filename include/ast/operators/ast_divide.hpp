#ifndef ast_divide_hpp
#define ast_divide_hpp

#include "ast_operator.hpp"


/*
 *  Node for divide.
*/
class Divide : public Operator
{
public:
    using Operator::Operator;

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES* indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        dst << " / ";
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("Mul::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string dest_reg,
        Context &context
    ) const override {
        throw std::runtime_error("Mul::divide_asm() not implemented");
    }
};


#endif  /* ast_divide_hpp */

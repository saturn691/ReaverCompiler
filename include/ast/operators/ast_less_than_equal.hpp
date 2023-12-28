#ifndef ast_less_than_equal_hpp
#define ast_less_than_equal_hpp

#include "ast_operator.hpp"


/*
 *  Node for less than equal.
*/
class LessThanEqual : public Operator
{
public:
    using Operator::Operator;

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES* indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        dst << " <= ";
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("evaluate not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        std::string temp_reg1 = context.allocate_register(Types::INT);
        std::string temp_reg2 = context.allocate_register(Types::INT);

        get_left()->gen_asm(dst, temp_reg1, context);
        get_right()->gen_asm(dst, temp_reg2, context);

        /* See note in ast_equal.hpp for information about 'andi' */

        // TODO handle multiple types
        dst << indent << "sgt " << dest_reg
            << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;
        dst << indent << "xori " << dest_reg
            << ", " << dest_reg << ", 1" << std::endl;

        context.deallocate_register(temp_reg1);
        context.deallocate_register(temp_reg2);
    }
};


#endif  /* ast_less_than_equal */

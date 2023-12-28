#ifndef ast_logical_or_hpp
#define ast_logical_or_hpp

#include "ast_operator.hpp"


/*
 *  Node for Logical OR.
 *  Logical OR is defined as: (a > 0) || (b > 0)
*/
class LogicalOr : public Operator
{
public:
    using Operator::Operator;

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES* indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        dst << " || ";
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string dest_reg,
        Context &context
    ) const override {
        /*
        A note from William: GCC compiles the equals as the following:
            ; x || y (a4 || a5)
        	    lw	a5,-20(s0)
                bne	a5,zero,.L2
                lw	a5,-24(s0)
                beq	a5,zero,.L3
            .L2:
                li	a5,1
                j	.L5
            .L3:
                li	a5,0
            .L5:
                mv	a0,a5

        This is quite inefficient, but let's look at the -O1 optimization:
        ; x || y (a4 || a5)
            or	    a0,a0,a1
	        snez	a0,a0

        As you can see, there is no need to branch anymore, as the result is
        just 1 when (a0 | a1 != 0).

        There may be a need to test for the signed case, but this works for now.
        */

        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        std::string temp_reg1 = context.allocate_register(Types::INT);
        std::string temp_reg2 = context.allocate_register(Types::INT);

        get_left()->gen_asm(dst, temp_reg1, context);
        get_right()->gen_asm(dst, temp_reg2, context);

        // TODO handle multiple types
        dst << indent << "or " << dest_reg
            << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;
        dst << indent << "snez " << dest_reg << ", " << dest_reg << std::endl;

        context.deallocate_register(temp_reg1);
        context.deallocate_register(temp_reg2);
    }
};


#endif  /* ast_logical_or */

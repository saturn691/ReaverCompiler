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
        std::string &dest_reg,
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

        This is quite inefficient, but it's a good starting point.
        The -O1 optimization was explored, but it is not very flexible for
        floating point numbers.
        */

        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        Types type = get_type(context);

        std::string temp_reg = context.allocate_register(Types::INT);
        std::string float_temp_reg = context.allocate_register(Types::FLOAT);
        std::string float_zero_reg = context.allocate_register(Types::FLOAT);

        std::string label1 = context.get_unique_label("LOGICAL_OR");
        std::string label2 = context.get_unique_label("LOGICAL_OR");
        std::string label_end = context.get_unique_label("LOGICAL_OR");

        switch (type)
        {
            case Types::FLOAT:
            case Types::DOUBLE:
            case Types::LONG_DOUBLE:
                get_left()->gen_asm(dst, float_temp_reg, context);
                dst << indent << "fmv.w.x " << float_zero_reg
                    << ", zero" << std::endl;
                dst << indent << "feq.s " << temp_reg
                    << ", " << float_temp_reg
                    << ", " << float_zero_reg << std::endl;
                dst << indent << "beq " << temp_reg
                    << ", zero, " << label1 << std::endl;
                break;

            default:
            get_left()->gen_asm(dst, temp_reg, context);
                dst << indent << "bne " << temp_reg
                    << ", zero, " << label1 << std::endl;
        }

        switch (type)
        {
            case Types::FLOAT:
            case Types::DOUBLE:
            case Types::LONG_DOUBLE:
                get_right()->gen_asm(dst, float_temp_reg, context);
                dst << indent << "feq.s " << temp_reg
                    << ", " << float_temp_reg
                    << ", " << float_zero_reg << std::endl;
                dst << indent << "bne " << temp_reg
                    << ", zero, " << label2 << std::endl;
                break;

            default:
                get_right()->gen_asm(dst, temp_reg, context);
                dst << indent << "beq " << temp_reg
                    << ", zero, " << label2 << std::endl;
        }

        dst << label1 << ":" << std::endl;
        dst << indent << "li " << temp_reg << ", 1" << std::endl;
        dst << indent << "j " << label_end << std::endl;

        dst << label2 << ":" << std::endl;
        dst << indent << "li " << temp_reg << ", 0" << std::endl;

        dst << label_end << ":" << std::endl;
        dst << indent << "mv " << dest_reg << ", "
            << temp_reg << std::endl;

        context.deallocate_register(temp_reg);
        context.deallocate_register(float_temp_reg);
        context.deallocate_register(float_zero_reg);
    }
};


#endif  /* ast_logical_or */

#ifndef ast_logical_and_hpp
#define ast_logical_and_hpp

#include "ast_operator.hpp"


/*
 *  Node for Logical AND.
 *  Logical AND is defined as: (a > 0) && (b > 0)
*/
class LogicalAnd : public Operator
{
public:
    using Operator::Operator;

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES* indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        dst << " && ";
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        /*
        A note from William: GCC compiles the equals as the following:
            ; x && y (a4 && a5)
                lw	a5,-20(s0)
                beq	a5,zero,.L2
                lw	a5,-24(s0)
                beq	a5,zero,.L2
                li	a5,1
                j	.L4
            .L2:
                li	a5,0
            .L4:
                mv	a0,a5

        One optimiation is to immediately put the result into the destination
        register instead of a5.

        There are more efficient ways to optimize this, such as using the
        "snez" instruction, but this is a good enough for now.
        */


        Types type = get_type(context);
        context.mode_stack.push(Context::Mode::OPERATOR);

        std::string temp_reg = context.allocate_register(
            dst, Types::INT, {dest_reg});
        std::string float_temp_reg = context.allocate_register(
            dst, Types::FLOAT, {dest_reg});
        std::string float_zero_reg = context.allocate_register(
            dst, Types::FLOAT, {dest_reg, float_temp_reg});

        std::string label1 = context.get_unique_label("LOGICAL_AND");
        std::string label2 = context.get_unique_label("LOGICAL_AND");

        switch (type)
        {
            case Types::FLOAT:
            case Types::DOUBLE:
            case Types::LONG_DOUBLE:
                get_left()->gen_asm(dst, float_temp_reg, context);
                // Set up the zero register for floating point numbers.
                dst << AST_INDENT << "fmv.w.x " << float_zero_reg
                    << ", zero" << std::endl;

                dst << AST_INDENT << "feq.s " << temp_reg
                    << ", " << float_zero_reg
                    << ", " << float_temp_reg << std::endl;
                dst << AST_INDENT << "bne " << temp_reg
                    << ", zero, " << label1 << std::endl;
                break;

            default:
                get_left()->gen_asm(dst, temp_reg, context);
                dst << AST_INDENT << "beq " << temp_reg
                    << ", zero, " << label1 << std::endl;
        }

        switch (type)
        {
            case Types::FLOAT:
            case Types::DOUBLE:
            case Types::LONG_DOUBLE:
                get_right()->gen_asm(dst, float_temp_reg, context);
                dst << AST_INDENT << "feq.s " << temp_reg
                    << ", " << float_zero_reg
                    << ", " << float_temp_reg << std::endl;
                dst << AST_INDENT << "bne " << temp_reg
                    << ", zero, " << label1 << std::endl;
                break;

            default:
                get_right()->gen_asm(dst, temp_reg, context);
                dst << AST_INDENT << "beq " << temp_reg
                    << ", zero, " << label1 << std::endl;
        }

        // Put the result into the destination register - see note above.
        dst << AST_INDENT << "li " << dest_reg << ", 1" << std::endl;
        dst << AST_INDENT << "j " << label2 << std::endl;

        dst << label1 << ":" << std::endl;
        dst << AST_INDENT << "li " << dest_reg << ", 0" << std::endl;

        dst << label2 << ":" << std::endl;

        context.mode_stack.pop();
        context.deallocate_register(dst, temp_reg);
    }
};


#endif  /* ast_logical_and */

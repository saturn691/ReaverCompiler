#ifndef ast_bitwise_xor_hpp
#define ast_bitwise_xor_hpp

#include "ast_operator.hpp"


/*
 *  Node for Bitwise XOR.
*/
class BitwiseXor : public Operator
{
public:
    using Operator::Operator;

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES* indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        dst << " ^ ";
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string temp_reg1 = context.allocate_register(Types::INT);
        std::string temp_reg2 = context.allocate_register(Types::INT);
        context.mode_stack.push(Context::Mode::OPERATOR);


        get_left()->gen_asm(dst, temp_reg1, context);
        get_right()->gen_asm(dst, temp_reg2, context);

        dst << AST_INDENT << "xor " << dest_reg
            << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;

        context.mode_stack.pop();
        context.deallocate_register(temp_reg1);
        context.deallocate_register(temp_reg2);
    }
};


#endif  /* ast_bitwise_or */

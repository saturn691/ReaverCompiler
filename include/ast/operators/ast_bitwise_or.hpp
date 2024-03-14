#ifndef ast_bitwise_or_hpp
#define ast_bitwise_or_hpp

#include "ast_operator.hpp"


/*
 *  Node for Bitwise OR.
*/
class BitwiseOr : public Operator
{
public:
    using Operator::Operator;

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES* indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        dst << " | ";
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        std::string temp_reg1 = context.allocate_register(Types::INT);
        std::string temp_reg2 = context.allocate_register(Types::INT);
        Context::Mode mode = context.mode;
        context.mode = Context::Mode::GLOBAL;

        get_left()->gen_asm(dst, temp_reg1, context);
        get_right()->gen_asm(dst, temp_reg2, context);

        dst << indent << "or " << dest_reg
            << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;

        context.mode = mode;
        context.deallocate_register(temp_reg1);
        context.deallocate_register(temp_reg2);
    }
};


#endif  /* ast_bitwise_or */

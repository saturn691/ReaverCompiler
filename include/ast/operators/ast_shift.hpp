#ifndef ast_shift_hpp
#define ast_shift_hpp

#include "ast_operator.hpp"
#include "../ast_context.hpp"


/*
 *  Node for left shift
 *  Example: "-" in "int x = a - b;"
*/
class LeftShift : public Operator
{
public:
    using Operator::Operator;

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES* indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        dst << " << ";
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        std::string temp_reg1 = context.allocate_register(get_type(context));
        std::string temp_reg2 = context.allocate_register(get_type(context));

        get_left()->gen_asm(dst, temp_reg1, context);
        get_right()->gen_asm(dst, temp_reg2, context);

        /* Note that shifts are undefined for floating point types in C. */

        dst << indent << "sll " << dest_reg
            << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;

        context.deallocate_register(temp_reg1);
        context.deallocate_register(temp_reg2);
    }
};


/*
 *  Node for right shift
 *  Example: ">>" in "int x = a >> b;"
*/
class RightShift : public Operator
{
public:
    using Operator::Operator;

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES* indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        dst << " >> ";
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        std::string temp_reg1 = context.allocate_register(get_type(context));
        std::string temp_reg2 = context.allocate_register(get_type(context));

        get_left()->gen_asm(dst, temp_reg1, context);
        get_right()->gen_asm(dst, temp_reg2, context);

        /* Note that shifts are undefined for floating point types in C. */

        dst << indent << "srl " << dest_reg
            << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;

        context.deallocate_register(temp_reg1);
        context.deallocate_register(temp_reg2);
    }
};


#endif  /* ast_shift_hpp */

#ifndef ast_sub_hpp
#define ast_sub_hpp

#include "ast_operator.hpp"


/*
 *  Node for subtraction.
 *  Example: "-" in "int x = a - b;"
*/
class Sub : public Operator
{
public:
    using Operator::Operator;

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES* indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        dst << " - ";
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        Types type = get_type(context);
        context.multiply_pointer = true;
        context.mode_stack.push(Context::Mode::OPERATOR);

        std::string temp_reg1 = context.allocate_register(type);
        get_left()->gen_asm(dst, temp_reg1, context);

        std::string temp_reg2 = context.allocate_register(type);
        get_right()->gen_asm(dst, temp_reg2, context);

        gen_ins(dst, type, temp_reg1, temp_reg2, dest_reg, ins_map);

        context.mode_stack.pop();
        context.deallocate_register(temp_reg1);
        context.deallocate_register(temp_reg2);
        context.multiply_pointer = false;
    }
private:
    const std::unordered_map<Types, std::string> ins_map = {
        {Types::UNSIGNED_CHAR, "sub"},
        {Types::CHAR, "sub"},
        {Types::UNSIGNED_SHORT, "sub"},
        {Types::SHORT, "sub"},
        {Types::INT, "sub"},
        {Types::UNSIGNED_INT, "sub"},
        {Types::LONG, "sub"},
        {Types::UNSIGNED_LONG, "sub"},
        {Types::FLOAT, "fsub.s"},
        {Types::DOUBLE, "fsub.d"},
        {Types::LONG_DOUBLE, "fsub.d"}
    };
};


#endif  /* ast_sub_hpp */

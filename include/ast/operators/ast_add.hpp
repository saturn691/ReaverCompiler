#ifndef ast_add_hpp
#define ast_add_hpp

#include "ast_operator.hpp"


/*
 *  Node for addition.
 *  Example: "+" in "int x = a + b;"
*/
class Add : public Operator
{
public:
    using Operator::Operator;

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES * indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        dst << " + ";
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        Types type = get_type(context);

        context.multiply_pointer = true;

        std::string temp_reg1 = context.allocate_register(type);
        get_left()->gen_asm(dst, temp_reg1, context);

        std::string temp_reg2 = context.allocate_register(type);
        get_right()->gen_asm(dst, temp_reg2, context);

        gen_ins(dst, type, temp_reg1, temp_reg2, dest_reg, ins_map);

        context.deallocate_register(temp_reg1);
        context.deallocate_register(temp_reg2);
        context.multiply_pointer = false;
    }

private:
    const std::unordered_map<Types, std::string> ins_map = {
        {Types::UNSIGNED_CHAR, "add"},
        {Types::CHAR, "add"},
        {Types::UNSIGNED_SHORT, "add"},
        {Types::SHORT, "add"},
        {Types::INT, "add"},
        {Types::UNSIGNED_INT, "add"},
        {Types::UNSIGNED_LONG, "add"},
        {Types::LONG, "add"},
        {Types::FLOAT, "fadd.s"},
        {Types::DOUBLE, "fadd.d"},
        {Types::LONG_DOUBLE, "fadd.d"}
    };
};


#endif  /* ast_add_hpp */

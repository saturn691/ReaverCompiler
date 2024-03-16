#ifndef ast_mul_hpp
#define ast_mul_hpp

#include "ast_operator.hpp"


/*
 *  Node for multiplication.
*/
class Mul : public Operator
{
public:
    using Operator::Operator;

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES* indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        dst << " * ";
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        Types type = get_type(context);
        Context::Mode mode = context.mode;
        context.mode = Context::Mode::GLOBAL;

        std::string temp_reg1 = context.allocate_register(type);
        get_left()->gen_asm(dst, temp_reg1, context);

        std::string temp_reg2 = context.allocate_register(type);
        get_right()->gen_asm(dst, temp_reg2, context);

        gen_ins(dst, type, temp_reg1, temp_reg2, dest_reg, ins_map);

        context.mode = mode;
        context.deallocate_register(temp_reg1);
        context.deallocate_register(temp_reg2);
    }

private:
    const std::unordered_map<Types, std::string> ins_map = {
        {Types::UNSIGNED_CHAR, "mul"},
        {Types::CHAR, "mul"},
        {Types::UNSIGNED_SHORT, "mul"},
        {Types::SHORT, "mul"},
        {Types::INT, "mul"},
        {Types::UNSIGNED_INT, "mul"},
        {Types::LONG, "mul"},
        {Types::UNSIGNED_LONG, "mul"},
        {Types::FLOAT, "fmul.s"},
        {Types::DOUBLE, "fmul.d"},
        {Types::LONG_DOUBLE, "fmul.d"}
    };

};


#endif  /* ast_mul_hpp */

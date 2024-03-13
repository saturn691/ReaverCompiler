#ifndef ast_divide_hpp
#define ast_divide_hpp

#include "ast_operator.hpp"


/*
 *  Node for divide.
*/
class Divide : public Operator
{
public:
    using Operator::Operator;

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES* indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        dst << " / ";
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        Types type = get_type();

        std::string temp_reg1 = context.allocate_register(type);
        std::string temp_reg2 = context.allocate_register(type);

        get_left()->gen_asm(dst, temp_reg1, context);
        get_right()->gen_asm(dst, temp_reg2, context);

        gen_ins(dst, type, temp_reg1, temp_reg2, dest_reg, ins_map);

        context.deallocate_register(temp_reg1);
        context.deallocate_register(temp_reg2);
    }

private:
    const std::unordered_map<Types, std::string> ins_map = {
        {Types::UNSIGNED_CHAR, "div"},
        {Types::CHAR, "div"},
        {Types::UNSIGNED_SHORT, "div"},
        {Types::SHORT, "div"},
        {Types::INT, "div"},
        {Types::UNSIGNED_INT, "div"},
        {Types::LONG, "div"},
        {Types::UNSIGNED_LONG, "div"},
        {Types::FLOAT, "fdiv.s"},
        {Types::DOUBLE, "fdiv.d"},
    };
};


#endif  /* ast_divide_hpp */

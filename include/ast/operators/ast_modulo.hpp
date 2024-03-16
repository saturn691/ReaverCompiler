#ifndef AST_MODULO_HPP
#define AST_MODULO_HPP

#include "ast_operator.hpp"


/*
 *  Node for modulo.
 *  Modulo is only defined for integer types.
*/
class Modulo : public Operator
{
public:
    using Operator::Operator;

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES* indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        dst << " % ";
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        // Cannot use normal get_type, only depends on the LHS
        Types type = get_left()->get_type(context);
        Context::Mode mode = context.mode;
        context.mode = Context::Mode::GLOBAL;

        // Undefined behaviour for floats/doubles
        std::string temp_reg1 = context.allocate_register(Types::INT);
        get_left()->gen_asm(dst, temp_reg1, context);

        std::string temp_reg2 = context.allocate_register(Types::INT);
        get_right()->gen_asm(dst, temp_reg2, context);

        gen_ins(dst, type, temp_reg1, temp_reg2, dest_reg, ins_map);

        context.mode = mode;
        context.deallocate_register(temp_reg1);
        context.deallocate_register(temp_reg2);
    }

private:
    const std::unordered_map<Types, std::string> ins_map = {
        {Types::UNSIGNED_CHAR, "remu"},
        {Types::CHAR, "rem"},
        {Types::UNSIGNED_SHORT, "remu"},
        {Types::SHORT, "rem"},
        {Types::INT, "rem"},
        {Types::UNSIGNED_INT, "remu"},
        {Types::LONG, "rem"},
        {Types::UNSIGNED_LONG, "remu"},
    };

};


#endif  /* AST_MODULO_HPP */

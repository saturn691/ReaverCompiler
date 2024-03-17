#ifndef ast_less_than_hpp
#define ast_less_than_hpp

#include "ast_operator.hpp"


/*
 *  Node for less than.
*/
class LessThan : public Operator
{
public:
    LessThan(
        Expression* _left,
        Expression* _right,
        bool _invert = false
    ) :
        Operator(_left, _right),
        invert(_invert)
    {}

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES* indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        if (invert)
        {
            dst << " >= ";
        }
        else
        {
            dst << " <= ";
        }
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        Types type = get_type(context);
        context.mode_stack.push(Context::Mode::OPERATOR);

        std::string temp_reg1 = context.allocate_register(
            dst, type, {dest_reg});
        std::string temp_reg2 = context.allocate_register(
            dst, type, {dest_reg, temp_reg1});

        get_left()->gen_asm(dst, temp_reg1, context);
        get_right()->gen_asm(dst, temp_reg2, context);

        /* See note in ast_equal.hpp for information about 'andi' */

        if (invert)
        {
            dst << AST_INDENT << ins_map.at(type) << " " << dest_reg
                << ", " << temp_reg2 << ", " << temp_reg1 << std::endl;
        }
        else
        {
            dst << AST_INDENT << ins_map.at(type) << " " << dest_reg
                << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;
        }

        context.mode_stack.pop();
        context.deallocate_register(dst, temp_reg1);
        context.deallocate_register(dst, temp_reg2);
    }

private:
    bool invert;
    const std::unordered_map<Types, std::string> ins_map = {
        {Types::UNSIGNED_CHAR, "slt"},
        {Types::CHAR, "slt"},
        {Types::UNSIGNED_SHORT, "slt"},
        {Types::SHORT, "slt"},
        {Types::INT, "slt"},
        {Types::UNSIGNED_INT, "slt"},
        {Types::LONG, "slt"},
        {Types::UNSIGNED_LONG, "slt"},
        {Types::FLOAT, "flt.s"},
        {Types::DOUBLE, "flt.d"},
        {Types::LONG_DOUBLE, "flt.d"}
    };
};


#endif  /* ast_less_than */

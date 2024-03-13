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

    virtual void print(std::ostream &dst, int indent_level) const override
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

    virtual void gen_asm(
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

        /* See note in ast_equal.hpp for information about 'andi' */

        if (invert)
        {
            gen_ins(dst, type, temp_reg2, temp_reg1, dest_reg, ins_map, true);
        }
        else
        {
            gen_ins(dst, type, temp_reg1, temp_reg2, dest_reg, ins_map, true);
        }

        context.deallocate_register(temp_reg1);
        context.deallocate_register(temp_reg2);
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

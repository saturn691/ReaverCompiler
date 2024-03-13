#ifndef ast_equal_hpp
#define ast_equal_hpp

#include "ast_operator.hpp"


/*
 *  Node for Equal.
*/
class Equal : public Operator
{
public:
    Equal(
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
            dst << " != ";
        }
        else
        {
            dst << " == ";
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

        /*
        A note from William: GCC compiles the equals as the following:
            ; x == y
            sub	    a5,a4,a5
            seqz	a5,a5
            andi	a5,a5,0xff
        However, I don't really see the point of the andi instruction.
        If something breaks down the line, try putting the andi back in.

        The solution below is VERY hacky, but it works! We use the subtract
        operator for most types, but for floating types they have their own
        operator, so we have to consider both cases before generating the
        return value.
        */

        gen_ins(dst, type, temp_reg1, temp_reg2, dest_reg, ins_map, true);

        std::string set_ins;
        if (((type == Types::FLOAT ||
            type == Types::DOUBLE ||
            type == Types::LONG_DOUBLE) && !invert) ||
            ((type != Types::FLOAT &&
            type != Types::DOUBLE &&
            type != Types::LONG_DOUBLE) && invert)
        ) {
            set_ins = "snez";
        }
        else
        {
            set_ins = "seqz";
        }

        dst << indent << set_ins << " " << dest_reg
            << ", " << dest_reg << std::endl;

        context.deallocate_register(temp_reg1);
        context.deallocate_register(temp_reg2);
    }

private:
    bool invert;
    const std::unordered_map<Types, std::string> ins_map = {
        {Types::UNSIGNED_CHAR, "sub"},
        {Types::CHAR, "sub"},
        {Types::UNSIGNED_SHORT, "sub"},
        {Types::SHORT, "sub"},
        {Types::INT, "sub"},
        {Types::UNSIGNED_INT, "sub"},
        {Types::LONG, "sub"},
        {Types::UNSIGNED_LONG, "sub"},
        {Types::FLOAT, "feq.s"},
        {Types::DOUBLE, "feq.d"},
        {Types::LONG_DOUBLE, "feq.d"}
    };
};


#endif  /* ast_equal */

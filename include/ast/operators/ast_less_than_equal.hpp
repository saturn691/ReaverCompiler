#ifndef ast_less_than_equal_hpp
#define ast_less_than_equal_hpp

#include "ast_operator.hpp"


/*
 *  Node for less than equal.
*/
class LessThanEqual : public Operator
{
public:
    LessThanEqual(
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
            dst << " > ";
        }
        else
        {
            dst << " < ";
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

        // Invert the result
        switch (type)
        {
            case Types::FLOAT:
            case Types::DOUBLE:
            case Types::LONG_DOUBLE:
                dst << AST_INDENT << "snez " << dest_reg
                    << ", " << dest_reg << std::endl;
                break;

            default:
                dst << AST_INDENT << "xori " << dest_reg
                    << ", " << dest_reg << ", 1" << std::endl;
        }

        context.mode_stack.pop();
        context.deallocate_register(temp_reg1);
        context.deallocate_register(temp_reg2);
    }

private:
    bool invert;
    const std::unordered_map<Types, std::string> ins_map = {
        {Types::UNSIGNED_CHAR, "sgt"},
        {Types::CHAR, "sgt"},
        {Types::UNSIGNED_SHORT, "sgt"},
        {Types::SHORT, "sgt"},
        {Types::INT, "sgt"},
        {Types::UNSIGNED_INT, "sgt"},
        {Types::LONG, "sgt"},
        {Types::UNSIGNED_LONG, "sgt"},
        {Types::FLOAT, "fle.s"},
        {Types::DOUBLE, "fle.d"},
        {Types::LONG_DOUBLE, "fle.d"}
    };

};


#endif  /* ast_less_than_equal */

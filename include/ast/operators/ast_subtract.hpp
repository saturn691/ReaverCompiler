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

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES* indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        dst << " - ";
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    virtual double evaluate(Context &context) const override
    {
        double left_val = get_left()->evaluate(context);
        double right_val = get_right()->evaluate(context);
        return left_val - right_val;
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        Types type = get_type(context);
        context.multiply_pointer = true;

        std::string temp_reg1 = context.allocate_register(type);
        get_left()->gen_asm(dst, temp_reg1, context);

        std::string temp_reg2 = context.allocate_register(type);
        get_right()->gen_asm(dst, temp_reg2, context);

        switch (type)
        {
            case Types::INT:
            case Types::UNSIGNED_INT:
                dst << indent << "sub " << dest_reg
                    << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;
                break;

            case Types::FLOAT:
                if (dest_reg[0] != 'f')
                {
                    // ".s" refers to single precision floating point
                    // Put into floating point register
                    // THEN move to dest register
                    dst << indent << "fsub.s " << temp_reg1
                        << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;

                    dst << indent << "fmv.s " << dest_reg
                        << ", " << temp_reg1 << std::endl;
                }
                else
                {
                    dst << indent << "fsub.s " << dest_reg
                        << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;
                }
                break;

            // TODO handle multiple types
            default:
                throw std::runtime_error("Subtract::gen_asm() not implemented");
        }

        context.deallocate_register(temp_reg1);
        context.deallocate_register(temp_reg2);
        context.multiply_pointer = false;
    }
};


#endif  /* ast_sub_hpp */

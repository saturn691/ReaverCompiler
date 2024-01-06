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

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES* indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        dst << " * ";
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("Mul::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        Types type = get_type(context);

        std::string temp_reg1 = context.allocate_register(type);
        get_left()->gen_asm(dst, temp_reg1, context);

        std::string temp_reg2 = context.allocate_register(type);
        get_right()->gen_asm(dst, temp_reg2, context);

        switch (type)
        {
            case Types::INT:
            case Types::UNSIGNED_INT:
                dst << indent << "mul " << dest_reg
                    << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;
                break;

            case Types::FLOAT:
                if (dest_reg[0] != 'f')
                {
                    // ".s" refers to single precision floating point
                    // Put into floating point register
                    // THEN move to dest register
                    dst << indent << "fmul.s " << temp_reg1
                        << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;

                    dst << indent << "fmv.s " << dest_reg
                        << ", " << temp_reg1 << std::endl;
                }
                else
                {
                    dst << indent << "fmul.s " << dest_reg
                        << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;
                }
                break;


            case Types::DOUBLE:
                if (dest_reg[0] != 'f')
                {
                    // ".d" refers to double precision floating point
                    dst << indent << "fmul.d " << temp_reg1
                        << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;

                    dst << indent << "fmv.d " << dest_reg
                        << ", " << temp_reg1 << std::endl;
                }
                else
                {
                    dst << indent << "fmul.d " << dest_reg
                        << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;
                }
                break;

            // TODO handle multiple types
            default:
                throw std::runtime_error("Subtract::gen_asm() not implemented");
        }

        context.deallocate_register(temp_reg1);
        context.deallocate_register(temp_reg2);
    }
};


#endif  /* ast_mul_hpp */

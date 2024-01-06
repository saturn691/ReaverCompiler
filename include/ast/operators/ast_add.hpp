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
        std::string indent((AST_PRINT_INDENT_SPACES* indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        dst << " + ";
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("Add::evaluate() not implemented");
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
                dst << indent << "add " << dest_reg
                    << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;
                break;

            case Types::FLOAT:
                if (dest_reg[0] != 'f')
                {
                    // ".s" refers to single precision floating point
                    // Put into floating point register
                    // THEN move to dest register
                    dst << indent << "fadd.s " << temp_reg1
                        << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;

                    dst << indent << "fmv.s " << dest_reg
                        << ", " << temp_reg1 << std::endl;
                }
                else
                {
                    dst << indent << "fadd.s " << dest_reg
                        << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;
                }
                break;

            case Types::DOUBLE:
                if (dest_reg[0] != 'f')
                {
                    // ".d" refers to double precision floating point
                    dst << indent << "fadd.d " << temp_reg1
                        << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;

                    dst << indent << "fmv.d " << dest_reg
                        << ", " << temp_reg1 << std::endl;
                }
                else
                {
                    dst << indent << "fadd.d " << dest_reg
                        << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;
                }
                break;

            // TODO handle multiple types
            default:
                throw std::runtime_error("Add::gen_asm() not implemented");
        }

        context.deallocate_register(temp_reg1);
        context.deallocate_register(temp_reg2);
    }
};


#endif  /* ast_add_hpp */

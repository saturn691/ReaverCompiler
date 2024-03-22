#ifndef ast_function_call_hpp
#define ast_function_call_hpp

#include "../ast_node.hpp"
#include "../operators/ast_operator.hpp"
#include "../ast_expression.hpp"
#include "../primitives/ast_identifier.hpp"


/*
 *  Node for defining function calls (e.g. "f(x, y)")
*/
class FunctionCall : public Expression
{
public:
    // No arguments provided into the function definition
    FunctionCall(
        Identifier* _identifier,
        NodeList* _argument_expression_list
    ) :
        identifier(_identifier),
        argument_expression_list(_argument_expression_list)
    {}

    virtual ~FunctionCall()
    {
        delete identifier;
        delete argument_expression_list;
    }

    Types get_type(Context &context) const override
    {
        return identifier->get_type(context);
    }

    std::string get_id() const override
    {
        return identifier->get_id();
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        identifier->print(dst, 0);
        dst << "(";
        if (argument_expression_list)
        {
            argument_expression_list->print(dst, 0);
        }
        dst << ");" << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string id = identifier->get_id();
        Types type = context.get_type(id);

        // Triggers the generation of assembly for the function parameters.
        std::string input_reg = "don't care";

        // Spill all registers, including a0
        context.push_registers(dst, dest_reg);

        // First argument goes into a0
        if (argument_expression_list)
        {
            argument_expression_list->gen_asm(dst, input_reg, context);
        }

        dst << AST_INDENT << "call " << id << std::endl;

        switch (type)
        {
            case Types::FLOAT:
            case Types::DOUBLE:
            case Types::LONG_DOUBLE:
                if (dest_reg != "fa0")
                {
                    Operator::move_reg(dst, "fa0", dest_reg, type, type);
                }
                else
                {
                    // Reallocate the return register
                    context.allocate_return_register(type);
                }

                break;

            default:
                if (dest_reg != "a0")
                {
                    Operator::move_reg(dst, "a0", dest_reg, type, type);
                }
                else
                {
                    // Reallocate the return register
                    context.allocate_return_register(type);
                }
                break;
        }

        // Restore the registers, after moving a0 to dest_reg
        context.pop_registers(dst);
    }

private:
    Identifier* identifier;
    NodeList* argument_expression_list;
};


#endif  /* ast_function_call_hpp */

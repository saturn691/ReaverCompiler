#ifndef ast_function_call_hpp
#define ast_function_call_hpp

#include "../ast_node.hpp"
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
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        std::string id = identifier->get_id();
        Types type = context.get_type(id);

        // Triggers the generation of assembly for the function parameters.
        std::string input_reg = "don't care";

        // First argument goes into a0
        if (argument_expression_list)
        {
            argument_expression_list->gen_asm(dst, input_reg, context);
        }

        context.push_registers(dst);
        dst << indent << "call " << id << std::endl;
        context.pop_registers(dst);

        switch (type)
        {
            case Types::FLOAT:
            case Types::DOUBLE:
            case Types::LONG_DOUBLE:
                dst << indent << "fmv.s " << dest_reg
                    << ", " << "fa0" << std::endl;
                break;

            default:
                dst << indent << "mv " << dest_reg
                    << ", " << "a0" << std::endl;
                break;
        }
    }

private:
    Identifier* identifier;
    NodeList* argument_expression_list;
};


#endif  /* ast_function_call_hpp */

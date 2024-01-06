#ifndef ast_function_call_hpp
#define ast_function_call_hpp

#include "../ast_node.hpp"


/*
 *  Node for defining function calls (e.g. "f(x, y)")
*/
class FunctionCall : public Node
{
public:
    // No arguments provided into the function definition
    FunctionCall(
        NodePtr _postfix_expression,
        NodePtr _argument_expression_list
    ) :
        postfix_expression(_postfix_expression),
        argument_expression_list(_argument_expression_list)
    {}

    virtual ~FunctionCall()
    {
        delete postfix_expression;
        delete argument_expression_list;
    }

    virtual std::string get_id() const override
    {
        return postfix_expression->get_id();
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        postfix_expression->print(dst, 0);
        dst << "(";
        if (argument_expression_list)
        {
            argument_expression_list->print(dst, 0);
        }
        dst << ");" << std::endl;
    }

    virtual Types get_type(Context& context) const override
    {
        return context.get_type(get_id());
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("FunctionCall::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');

        // Triggers the generation of assembly for the function parameters.
        std::string input_reg = "don't care";
        Types type = get_type(context);

        // First argument goes into a0
        if (argument_expression_list)
        {
            argument_expression_list->gen_asm(dst, input_reg, context);
        }

        context.push_registers(dst);
        dst << indent << "call " << get_id() << std::endl;
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
    NodePtr postfix_expression;
    NodePtr argument_expression_list;
};


#endif  /* ast_function_call_hpp */

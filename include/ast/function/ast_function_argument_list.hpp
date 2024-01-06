#ifndef ast_function_argument_list_hpp
#define ast_function_argument_list_hpp

#include "../ast_binary_node.hpp"


/*
 *  Node for defining function argument list as part of function calls
 *  Example: "x, y, z" in f(x, y, z);
*/
class FunctionArgumentList : public BinaryNode
{
public:
    using BinaryNode::BinaryNode;

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        left->print(dst, 0);

        if (right)
        {
            dst << ", ";
            right->print(dst, 0);
        }
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("FunctionArgumentList::evaluate() not implemented");
    }

    virtual Types get_type(Context &context) const override
    {
        return left->get_type(context);
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        context.deallocate_arg_registers();

        // The function parameter list is held in the LEFT node
        std::string arg_reg = context.allocate_arg_register(get_type(context));
        left->gen_asm(dst, arg_reg, context);
        if (right)
        {
            arg_reg = context.allocate_arg_register(right->get_type(context));
            right->gen_asm(dst, arg_reg, context);
        }
    }
};


#endif  /* ast_function_argument_list_hpp */

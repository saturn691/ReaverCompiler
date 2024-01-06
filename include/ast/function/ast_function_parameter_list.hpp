#ifndef ast_function_parameter_list_hpp
#define ast_function_parameter_list_hpp

#include "../ast_binary_node.hpp"


/*
 *  Node for defining function parameter list
 *  Example: "int x, int y, int z" in f(int x, int y, int z) { return x; }
*/
class FunctionParameterList : public BinaryNode
{
public:
    using BinaryNode::BinaryNode;
    virtual void print(std::ostream &dst, int indent_level) const override
    {
        left->print(dst, 0);
        dst << ", ";
        right->print(dst, 0);
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("FunctionParameterList::evaluate() not implemented");
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
        left->gen_asm(dst, dest_reg, context);
        right->gen_asm(dst, dest_reg, context);
    }
};


#endif  /* ast_function_parameter_list_hpp */

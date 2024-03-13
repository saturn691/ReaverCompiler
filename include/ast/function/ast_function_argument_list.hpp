#ifndef ast_function_argument_list_hpp
#define ast_function_argument_list_hpp

#include "../ast_node.hpp"
#include "../ast_expression.hpp"


/*
 *  Node for defining function argument list as part of function calls
 *  Example: "x, y, z" in f(x, y, z);
*/
class FunctionArgumentList : public NodeList
{
public:
    FunctionArgumentList(
        Expression* _node
    ) :
        nodes({_node})
    {}

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        context.deallocate_arg_registers();

        for (auto &node : nodes)
        {
            std::string arg_reg = context.allocate_arg_register(node->get_type());
            node->gen_asm(dst, arg_reg, context);
        }
    }

    void push_back(Expression* _node)
    {
        nodes.push_back(_node);
    }

private:
    std::vector<Expression*> nodes;
};


#endif  /* ast_function_argument_list_hpp */

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
    using NodeList::NodeList;

    void print(std::ostream &dst, int indent_level) const override
    {
        for (auto &node : nodes)
        {
            node->print(dst, indent_level);
            dst << ", ";
        }
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        context.deallocate_arg_registers();

        // Cast to Expression (assignment_expression)
        for (auto &node : nodes)
        {
            Expression* n = dynamic_cast<Expression*>(node);
            std::string arg_reg = context
                .allocate_arg_register(n->get_type(context));

            node->gen_asm(dst, arg_reg, context);
        }
    }

    void push_back(Expression* _node)
    {
        nodes.push_back(_node);
    }
};


#endif  /* ast_function_argument_list_hpp */

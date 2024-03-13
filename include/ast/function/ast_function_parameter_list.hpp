#ifndef ast_function_parameter_list_hpp
#define ast_function_parameter_list_hpp

#include "../ast_binary_node.hpp"


/*
 *  Node for defining function parameter list
 *  Example: "int x, int y, int z" in f(int x, int y, int z) { return x; }
*/
class FunctionParameterList : public NodeList
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

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        context.deallocate_arg_registers();

        NodeList::gen_asm(dst, dest_reg, context);
    }
private:
    std::vector<Node*> nodes;
};


#endif  /* ast_function_parameter_list_hpp */

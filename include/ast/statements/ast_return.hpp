#ifndef ast_return_hpp
#define ast_return_hpp

#include "../ast_node.hpp"


/*
 *  Node for returning (e.g. "return 5;" in "int f() { return 5; }")
*/
class Return : public Node
{
public:
    Return(NodePtr _return_node) : return_node(_return_node)
    {}

    virtual ~Return()
    {
        delete return_node;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "return" << std::endl;
        dst << indent << "(" << std::endl;
        return_node->print(dst, indent_level + 1);
        dst << indent << ")" << std::endl;
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("Return::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');

        return_node->gen_asm(dst, dest_reg, context);
        dst << indent << "j end" << std::endl;
    }

private:
    NodePtr return_node;
};


#endif  /* ast_return_hpp */

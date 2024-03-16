#ifndef ast_return_hpp
#define ast_return_hpp

#include "../ast_node.hpp"


/*
 *  Node for returning (e.g. "return 5;" in "int f() { return 5; }")
*/
class Return : public Node
{
public:
    Return(Node* _return_node) : return_node(_return_node)
    {}

    virtual ~Return()
    {
        delete return_node;
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "return" << std::endl;
        dst << AST_INDENT << "(" << std::endl;
        return_node->print(dst, indent_level + 1);
        dst << AST_INDENT << ")" << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        context.mode = Context::Mode::RETURN;
        return_node->gen_asm(dst, dest_reg, context);
        context.mode = Context::Mode::GLOBAL;

        dst << AST_INDENT << "j " << context.current_id << "_end" << std::endl;
    }

private:
    Node* return_node;
};


#endif  /* ast_return_hpp */

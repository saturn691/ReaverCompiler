#ifndef ast_break_hpp
#define ast_break_hpp

#include "../ast_node.hpp"
#include "../ast_context.hpp"


/*
 *  Node for break statements
*/
class Break : public Node
{
public:
    Break()
    {}

    virtual ~Break()
    {}

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "break;" << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        dst << AST_INDENT << "j "
            << context.end_label_stack.top() << std::endl;
    }
};


#endif  /* ast_break_hpp */

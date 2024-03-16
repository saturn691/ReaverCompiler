#ifndef AST_CONTINUE_HPP
#define AST_CONTINUE_HPP

#include "../ast_node.hpp"
#include "../ast_context.hpp"


/*
 *  Node for continue statements. See section 6.6.6.2 of the C90 spec.
 *  Defined in each file for
 *  1. while,
 *  2. do-while
 *  3. for loops.
*/
class Continue : public Node
{
public:
    Continue()
    {}

    virtual ~Continue()
    {}

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "continue;" << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        dst << AST_INDENT << "j "
            << context.continue_label_stack.top() << std::endl;
    }
};


#endif  /* AST_CONTINUE_HPP */

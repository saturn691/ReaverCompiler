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

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "break;" << std::endl;
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');

        if (context.mode == Context::Mode::SWITCH)
        {
            // Break from default cases are ignored
            context.switch_cases.second << indent << "j "
                << context.end_label_stack.top() << std::endl;
            return;
        }
        else
        {
            dst << indent << "j "
                << context.end_label_stack.top() << std::endl;
        }
    }
};


#endif  /* ast_break_hpp */
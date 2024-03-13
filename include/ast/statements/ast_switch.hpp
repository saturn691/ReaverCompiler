#ifndef ast_switch_hpp
#define ast_switch_hpp

#include "../ast_node.hpp"
#include "../ast_context.hpp"


/*
 *  Node for switch-case statements
 *  (e.g. "switch (x) { case 1: return 1; }")
*/
class Switch : public Node
{
public:
    Switch(
        Expression* _expression,
        Node* _statement
    ) :
        expression(_expression),
        statement(_statement)
    {}

    virtual ~Switch()
    {
        delete expression;
        delete statement;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "switch (";
        expression->print(dst, 0);
        dst << ")" << std::endl;
        dst << indent << "{" << std::endl;
        statement->print(dst, indent_level + 1);
        dst << indent << "}" << std::endl;
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        /*
        Note: GCC compiles the switch case statements like so

        expression
        conditions
        labels:
            statement
            (break = jump to the end)
        end_label
        */
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        context.mode = Context::Mode::SWITCH;
        std::string end_label = context.get_unique_label("switch_end");

        // Imperative for breaking out of the statement
        context.end_label_stack.push(end_label);

        std::string switch_reg = context.allocate_register(
            expression->get_type()
        );
        context.switch_reg = switch_reg;

        expression->gen_asm(dst, switch_reg, context);
        statement->gen_asm(dst, dest_reg, context);

        // Output the switch cases
        dst << context.switch_cases.first.str();
        dst << context.switch_default.first.str();
        dst << context.switch_cases.second.str();
        dst << context.switch_default.second.str();

        dst << end_label << ":" << std::endl;

        // Cleanup
        context.deallocate_register(switch_reg);
        context.switch_reg = "";
        context.mode = Context::Mode::GLOBAL;
        context.end_label_stack.pop();
    }

private:
    Expression* expression;
    Node* statement;
};


#endif  /* ast_switch_hpp */

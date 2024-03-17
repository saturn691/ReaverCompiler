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

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "switch (";
        expression->print(dst, 0);
        dst << ")" << std::endl;
        dst << AST_INDENT << "{" << std::endl;
        statement->print(dst, indent_level + 1);
        dst << AST_INDENT << "}" << std::endl;
    }

    void gen_asm(
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

        std::stringstream statement_ss;
        context.mode_stack.push(Context::Mode::SWITCH);
        std::string end_label = context.get_unique_label("switch_end");

        // Save the outer switch context and clean the inner switch context
        std::string outer_switch_reg = context.switch_reg;
        std::stringstream outer_switch_cases_expr;
        outer_switch_cases_expr << context.switch_cases_expr.str();
        std::stringstream outer_switch_default;
        outer_switch_default << context.switch_default.str();
        context.switch_cases_expr.str("");
        context.switch_default.str("");

        // Imperative for breaking out of the statement
        context.end_label_stack.push(end_label);

        std::string switch_reg = context.allocate_register(
            dst, expression->get_type(context), {dest_reg}
        );
        context.switch_reg = switch_reg;

        expression->gen_asm(dst, switch_reg, context);

        // Does not print anything yet, we need do some reordering
        statement->gen_asm(statement_ss, dest_reg, context);

        // Output the switch cases
        dst << context.switch_cases_expr.str();

        /**
         * Section 6.8.4.2, Paragraph 5
         * "If no converted case constant expression matches and there is
         * no default label, no part of the switch body is executed."
        */
        if (context.switch_default.str() != "")
        {
            dst << context.switch_default.str();
        }
        else
        {
            dst << AST_INDENT << "j " << end_label << std::endl;
        }

        dst << context.switch_default.str();
        dst << statement_ss.str();

        dst << end_label << ":" << std::endl;

        // Cleanup
        context.deallocate_register(dst, switch_reg);
        context.switch_reg = outer_switch_reg;
        context.switch_cases_expr.str("");
        context.switch_cases_expr << outer_switch_cases_expr.str();
        context.switch_default.str("");
        context.switch_default << outer_switch_default.str();
        context.mode_stack.pop();
        context.end_label_stack.pop();
    }

private:
    Expression* expression;
    Node* statement;
};


#endif  /* ast_switch_hpp */

#ifndef ast_while_hpp
#define ast_while_hpp

#include "../ast_node.hpp"


/*
 *  Node for while loops (e.g. "while (i < 10) { ... }")
*/
class While : public Node
{
public:
    While (
        Node* _condition,
        Node* _statement
    ) :
        condition(_condition),
        statement(_statement)
    {}

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "while" << std::endl;
        dst << AST_INDENT << "(" << std::endl;
        condition->print(dst, indent_level + 1); // condition
        dst << AST_INDENT << ")" << std::endl;
        dst << AST_INDENT << "{" << std::endl;
        if (statement)
        {
            statement->print(dst, indent_level + 1); // statement
        }
        dst << AST_INDENT << "}" << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string start_label = context.get_unique_label("while_start");
        std::string condition_label = context.get_unique_label("while_condition");
        std::string end_label = context.get_unique_label("while_end");

        context.continue_label_stack.push(condition_label);
        context.end_label_stack.push(end_label);

        std::string condition_reg = context.allocate_register(
            dst, Types::INT, {dest_reg});

        // Jump to condition
        dst << AST_INDENT << "j" << AST_INDENT << condition_label << std::endl;

        // STATEMENT
        dst << start_label << ":" << std::endl;
        if (statement)
        {
            statement->gen_asm(dst, dest_reg, context);
        }

        // CONDITION
        dst << condition_label << ":" << std::endl;
        if (condition)
        {
            condition->gen_asm(dst, condition_reg, context);
        }
        // This is unoptimised, but who cares :) just SSA it bro
        dst << AST_INDENT << "bnez " << condition_reg
            << ", " << start_label << std::endl;

        // END
        dst << end_label << ":" << std::endl;

        context.continue_label_stack.pop();
        context.end_label_stack.pop();
        context.deallocate_register(dst, condition_reg);
    }

private:
    Node* condition;
    Node* statement;
};


#endif  /* ast_while_hpp */

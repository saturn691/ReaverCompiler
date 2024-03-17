#ifndef AST_DO_WHILE_HPP
#define AST_DO_WHILE_HPP

#include "../ast_node.hpp"
#include "../ast_context.hpp"


/*
 *  Node for do-while loops (e.g. "while (i < 10) { ... }")
*/
class DoWhile : public Node
{
public:
    DoWhile (
        Node* _statement,
        Node* _condition
    ) :
        statement(_statement),
        condition(_condition)
    {}

    ~DoWhile() {
        delete statement;
        delete condition;
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(indent_level * AST_PRINT_INDENT_SPACES, ' ');

        dst << indent << "do(" << std::endl;
        statement->print(dst, indent_level + 1);
        dst << indent << ") " << std::endl;
        dst << indent;
        dst << "while (" << std::endl;
        condition->print(dst, indent_level + 1);
        dst << indent << ");" << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string start_label = context.get_unique_label("do_while");
        std::string condition_label = context.get_unique_label("do_while_condition");
        std::string end_label = context.get_unique_label("do_while_end");
        std::string condition_reg = context.allocate_register(
            dst, Types::INT, {dest_reg});

        context.continue_label_stack.push(condition_label);
        context.end_label_stack.push(end_label);

        // START
        dst << start_label << ":" << std::endl;
        if (statement)
        {
            statement->gen_asm(dst, dest_reg, context);
        }

        // CONDITION (continue points to here)
        dst << condition_label << ":" << std::endl;
        condition->gen_asm(dst, condition_reg, context);
        dst << AST_INDENT << "bnez " << condition_reg
            << ", " << start_label << std::endl;

        // END (break points to here)
        dst << end_label << ":" << std::endl;

        context.continue_label_stack.pop();
        context.end_label_stack.pop();
        context.deallocate_register(dst, condition_reg);
    }
private:
    Node* statement;
    Node* condition;
};


#endif  /* AST_DO_WHILE_HPP */

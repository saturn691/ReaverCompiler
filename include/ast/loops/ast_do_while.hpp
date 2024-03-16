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
        delete condition;
        delete statement;
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
        std::string end_label = context.get_unique_label("do_while_end");
        std::string condition_reg = context.allocate_register(Types::INT);

        context.continue_label_stack.push(start_label);
        context.end_label_stack.push(end_label);

        // START
        dst << start_label << ":" << std::endl;
        if (statement)
        {
            statement->gen_asm(dst, dest_reg, context);
        }

        // CONDITION (continue points to here)
        condition->gen_asm(dst, condition_reg, context);
        dst << AST_INDENT << "bnez " << condition_reg
            << ", " << start_label << std::endl;

        // END (break points to here)
        dst << end_label << ":" << std::endl;

        context.deallocate_register(condition_reg);
    }
private:
    Node* condition;
    Node* statement;
};


#endif  /* AST_DO_WHILE_HPP */
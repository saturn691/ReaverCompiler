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
        dst << indent << "(" << std::endl;
        condition->print(dst, indent_level + 1); // condition
        dst << indent << ")" << std::endl;
        dst << indent << "{" << std::endl;
        if (statement)
        {
            statement->print(dst, indent_level + 1); // statement
        }
        dst << indent << "}" << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');

        std::string start_label = context.get_unique_label("while");
        std::string end_label = context.get_unique_label("while");

        std::string condition_reg = context.allocate_register(Types::INT);

        dst << indent << "j" << indent << end_label << std::endl;
        dst << start_label << ":" << std::endl;
        if (statement)
        {
            statement->gen_asm(dst, dest_reg, context);
        }
        dst << end_label << ":" << std::endl;
        if (condition)
        {
            condition->gen_asm(dst, condition_reg, context);
        }
        // This is unoptimised, but who cares :) just SSA it bro
        dst << indent << "bnez " << condition_reg << ", " << start_label << std::endl;

        context.deallocate_register(condition_reg);
    }

private:
    Node* condition;
    Node* statement;
};


#endif  /* ast_while_hpp */

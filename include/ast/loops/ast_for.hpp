#ifndef ast_for_hpp
#define ast_for_hpp

#include "../ast_node.hpp"


/*
 *  Node for for loops (e.g. "for (int i = 0; i < 10; i++) { ... }")
*/
class For : public Node
{
public:
    // Constructor for 'for' loop with no iteration expression
    For(
        Node* _initiation,
        Node* _condition,
        Node* _statement
    ) :
        initiation(_initiation),
        condition(_condition),
        iteration(nullptr), // No iteration expression
        statement(_statement)
    {}

    // Constructor for 'for' loop with iteration expression
    For(
        Node* _initiation,
        Node* _condition,
        Node* _iteration,
        Node* _statement
    ) :
        initiation(_initiation),
        condition(_condition),
        iteration(_iteration),
        statement(_statement)
    {}

    virtual ~For(){
        delete initiation;
        delete condition;
        delete iteration;
        delete statement;
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "for" << std::endl;
        dst << indent << "(" << std::endl;
        initiation->print(dst, indent_level + 1); // initiation
        condition->print(dst, indent_level + 1); // condition
        dst << indent << ";" << std::endl;

        if (iteration != nullptr)
        {
            iteration->print(dst, indent_level + 1); // iteration
        }

        dst << indent << ")" << std::endl;
        dst << indent << "{" << std::endl;

        if (statement != nullptr)
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

        std::string start_label = context.get_unique_label("for_start");
        std::string end_label = context.get_unique_label("for_end");

        // "The continue statement used anywhere within the loop-statement
        //  transfers control to iteration-expression"
        context.continue_label_stack.push(start_label);
        context.end_label_stack.push(end_label);

        std::string loop_reg = context.allocate_register(Types::INT);

        // Loop initialization
        if (initiation)
        {
            initiation->gen_asm(dst, loop_reg, context);
        }

        dst << start_label << ":" << std::endl;

        // Loop condition
        if (condition)
        {
            condition->gen_asm(dst, loop_reg, context);
            dst << indent << "beqz " << loop_reg << ", " << end_label << std::endl;
        }

        // Loop statement
        if (statement)
        {
            statement->gen_asm(dst, dest_reg, context);
        }

        // Loop iteration
        if (iteration)
        {
            iteration->gen_asm(dst, loop_reg, context);
        }

        dst << indent << "j " << start_label << std::endl;
        dst << end_label << ":" << std::endl;

        // Clean up
        context.end_label_stack.pop();
        context.continue_label_stack.pop();
        context.deallocate_register(loop_reg);
    }

private:
    Node* initiation;
    Node* condition;
    Node* iteration;
    Node* statement;
};


#endif  /* ast_for_hpp */

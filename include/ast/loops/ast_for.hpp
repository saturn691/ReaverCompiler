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
        NodePtr _initiation,
        NodePtr _condition,
        NodePtr _statement
    ) :
        initiation(_initiation),
        condition(_condition),
        iteration(nullptr), // No iteration expression
        statement(_statement)
    {}

    // Constructor for 'for' loop with iteration expression
    For(
        NodePtr _initiation,
        NodePtr _condition,
        NodePtr _iteration,
        NodePtr _statement
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

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "for" << std::endl;
        dst << indent << "(" << std::endl;
        initiation->print(dst, indent_level + 1); // initiation
        condition->print(dst, indent_level + 1); // condition
        dst << indent << ";" << std::endl;
        if (iteration != nullptr) {
            iteration->print(dst, indent_level + 1); // iteration
        }
        dst << indent << ")" << std::endl;
        dst << indent << "{" << std::endl;
        statement->print(dst, indent_level + 1); // statement
        dst << indent << "}" << std::endl;
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("For::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        // TODO: idk fix this shit
        // TODO: Label naming for loops

        static int loop_counter = 0;  // Unique loop identifier
        int curr_loop = loop_counter++;

        std::string loop_start_label = "loop_start_" + std::to_string(curr_loop);
        std::string loop_end_label = "loop_end_" + std::to_string(curr_loop);

        std::string indent(AST_PRINT_INDENT_SPACES, ' ');

        // Loop initialization
        if (initiation) {
            initiation->gen_asm(dst, dest_reg, context);
        }

        dst << loop_start_label << ":" << std::endl;  // Loop start label

        // Loop condition
        if (condition) {
            condition->gen_asm(dst, dest_reg, context);
            dst << indent << "beqz " << dest_reg << ", " << loop_end_label << std::endl;
        }

        // Loop statement
        if (statement) {
            statement->gen_asm(dst, dest_reg, context);
        }

        // Loop iteration
        if (iteration) {
            iteration->gen_asm(dst, dest_reg, context);
        }

        // Jump back to start
        dst << indent << "j " << loop_start_label << std::endl;

        dst << loop_end_label << ":" << std::endl;  // Loop end label
    }

private:
    NodePtr initiation;
    NodePtr condition;
    NodePtr iteration;
    NodePtr statement;
};


#endif  /* ast_for_hpp */

#ifndef ast_assign_hpp
#define ast_assign_hpp

#include "../../ast_node.hpp"


/*
 *  Node for assignment (e.g. "int x = 5;")
*/
class Assign : public Node
{
public:
    Assign(
        NodePtr _unary_expression,
        NodePtr _assignment_operator,
        NodePtr _assignment_expression
    ) :
        unary_expression(_unary_expression),
        assignment_operator(_assignment_operator),
        assignment_expression(_assignment_expression)
    {}
    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES * indent_level), ' ');

        dst << indent;
        unary_expression->print(dst, 0);
        dst << " ";
        assignment_operator->print(dst, 0);
        dst << " ";
        assignment_expression->print(dst, 0);
        dst << ";" << std::endl;
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("Assign::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        std::string id = assignment_operator->get_id();
        int stack_loc = context.variable_map.at(
            unary_expression->get_id()).stack_location;

        // TODO consider other types
        // Put the assignment expression into a temporary register
        std::string reg = context.allocate_register(Types::INT);
        assignment_expression->gen_asm(dst, reg, context);

        // TODO implement for all assignment operators
        if (id == "=")
        {
            // TODO consider other types
            dst << indent << "sw " << reg << ", "
                << stack_loc << "(s0)" << std::endl;
        }
        else
        {
            throw std::runtime_error("Assign::gen_asm() not implemented");
        }

        context.deallocate_register(reg);
    }

private:
    NodePtr unary_expression;
    NodePtr assignment_operator;
    NodePtr assignment_expression;
};


#endif  /* ast_assign_hpp */

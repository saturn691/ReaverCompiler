#ifndef ast_unary_expression_hpp
#define ast_unary_expression_hpp

#include "../ast_node.hpp"
#include "../ast_context.hpp"


/*
 *  Node for unary expressions
 *  (e.g. "+x", "-x", "~x", "!x", "&x")
*/
class UnaryExpression : public Node
{
public:
    UnaryExpression(
        std::string _unary_operator,
        NodePtr _cast_expression
    ) :
        unary_operator(_unary_operator),
        cast_expression(_cast_expression)
    {}

    virtual ~UnaryExpression()
    {
        delete cast_expression;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent << unary_operator;
        cast_expression->print(dst, 0);
    }

    virtual Types get_type(Context &context) const override
    {
        return cast_expression->get_type(context);
    }

    virtual std::string get_id() const override
    {
        return cast_expression->get_id();
    }

    std::string get_unary_operator() const
    {
        return unary_operator;
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');

        if (unary_operator == "*")
        {
            context.is_pointer = true;
        }
        cast_expression->gen_asm(dst, dest_reg, context);

        if (unary_operator == "-")
        {
            dst << indent << "neg " << dest_reg << ", " << dest_reg << std::endl;
        }
        else if (unary_operator == "~")
        {
            dst << indent << "not " << dest_reg << ", " << dest_reg << std::endl;
        }
        else if (unary_operator == "!")
        {
            dst << indent << "seqz " << dest_reg << ", " << dest_reg << std::endl;
        }
        else if (unary_operator == "&") // for pointers -> address of
        {
            std::string id = cast_expression->get_id();
            int address = context.get_stack_location(id);
            dst << indent << "addi " << dest_reg << ", s0, " << address << std::endl;
        }
        else if (unary_operator == "*" && context.mode != Context::Mode::ASSIGN) // for pointers -> dereference
        {
            std::string load = Context::get_load_instruction(get_type(context));
            dst << indent << load << " " << dest_reg
                << ", 0(" << dest_reg << ")" << std::endl;
        }

        context.is_pointer = false;
    }

private:
    std::string unary_operator;
    NodePtr cast_expression;
};


#endif  /* ast_unary_expression_hpp */

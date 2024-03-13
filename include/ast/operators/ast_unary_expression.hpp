#ifndef ast_unary_expression_hpp
#define ast_unary_expression_hpp

#include "../ast_expression.hpp"
#include "../ast_context.hpp"


/*
 *  Node for unary expressions
 *  (e.g. "+x", "-x", "~x", "!x", "&x")
*/
class UnaryExpression : public Expression
{
public:
    UnaryExpression(
        std::string _unary_operator,
        Expression* _cast_expression
    ) :
        unary_operator(_unary_operator),
        cast_expression(_cast_expression)
    {}

    virtual ~UnaryExpression()
    {
        delete cast_expression;
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent << unary_operator;
        cast_expression->print(dst, 0);
    }

    std::string get_id() const override
    {
        return cast_expression->get_id();
    }

    Types get_type() const override
    {
        return cast_expression->get_type();
    }

    std::string get_unary_operator() const
    {
        return unary_operator;
    }

    void gen_asm(
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
            // TODO pass this onto cast_expression->gen_asm()
            // std::string id = cast_expression->get_id();
            // int address = context.get_stack_location(id);
            // dst << indent << "addi " << dest_reg << ", s0, " << address << std::endl;
        }
        // for pointers -> dereference
        else if (unary_operator == "*" && context.mode != Context::Mode::ASSIGN)
        {
            std::string load = Context::get_load_instruction(get_type());
            dst << indent << load << " " << dest_reg
                << ", 0(" << dest_reg << ")" << std::endl;
        }

        context.is_pointer = false;
    }

private:
    std::string unary_operator;
    Expression* cast_expression;
};


#endif  /* ast_unary_expression_hpp */

#ifndef ast_unary_expression_hpp
#define ast_unary_expression_hpp

#include "../ast_node.hpp"
#include "../ast_context.hpp"


/*
 *  Node for unary expressions
 *  (e.g. "+x", "-x", "~x", "!x")
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

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');

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
    }

private:
    std::string unary_operator;
    NodePtr cast_expression;
};


#endif  /* ast_unary_expression_hpp */
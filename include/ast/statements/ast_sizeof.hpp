#ifndef ast_sizeof_hpp
#define ast_sizeof_hpp

#include "../ast_node.hpp"
#include "../ast_context.hpp"

/*
 *  Defines the sizeof operator (e.g. "sizeof(int)")
*/
class SizeOf : public Expression
{
public:
    SizeOf(Node* _child) : child(_child) {}

    ~SizeOf()
    {
        delete child;
    }

    std::string get_id() const override
    {
        // Attempt to cast to Expression
        Expression* expression = dynamic_cast<Expression*>(child);

        if (expression != NULL)
        {
            return expression->get_id();
        }

        return "";
    }

    Types get_type(Context &context) const override
    {
        // Try casting to Type
        Type* type = dynamic_cast<Type*>(child);
        // Try casting to Expression
        Expression* expression = dynamic_cast<Expression*>(child);

        if (type != NULL)
        {
            return type->get_type();
        }
        else if (expression != NULL)
        {
            return expression->get_type(context);
        }
        else
        {
            return Types::VOID;
        }

    }

    double evaluate() const override
    {
        // Cast to expression and evaluate
        Expression* expression = dynamic_cast<Expression*>(child);
        if (expression != NULL)
        {
            return expression->evaluate();
        }

        // Cast to type and get type
        Type* type = dynamic_cast<Type*>(child);
        if (type != NULL)
        {
            return Context::type_size_map.at(type->get_type());
        }

        return 0;
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES * indent_level), ' ');

        dst << AST_INDENT << "sizeof(";
        child->print(dst, 0);
        dst << ")" << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        context.mode_stack.push(Context::Mode::SIZEOF);
        child->gen_asm(dst, dest_reg, context);
        context.mode_stack.pop();
    }

private:
    // unary_expression or type_name
    Node* child;
};


#endif  /* ast_sizeof_hpp */

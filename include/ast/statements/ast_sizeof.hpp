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

    Types get_type() const override
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
            return expression->get_type();
        }
        else
        {
            return Types::VOID;
        }

    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES * indent_level), ' ');

        dst << indent << "sizeof(";
        child->print(dst, 0);
        dst << ")" << std::endl;
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');

        // TODO this
        // unsigned int size = child->get_size(context);
        // dst << indent << "li " << dest_reg << ", " << size << std::endl;
    }

private:
    // unary_expression or type_name
    Node* child;
};


#endif  /* ast_sizeof_hpp */

#ifndef ast_enum_value_hpp
#define ast_enum_value_hpp

#include "../../ast_node.hpp"
#include "../../ast_context.hpp"


/*
 *  Defines an enum value
 *  (e.g. "a" in "enum x { a, b, c };")
*/
class EnumValue : public Node
{
public:
    EnumValue(
        std::string _identifier
    ) :
        identifier(_identifier)
    {}

    EnumValue(
        std::string _identifier,
        Node* _expression
    ) :
        identifier(_identifier),
        expression(_expression)
    {}

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES * indent_level), ' ');
        // Intentionally at the start. This is to do with commas
        dst << std::endl;
        dst << AST_INDENT << identifier;
        if (expression != NULL) {
            dst << " = ";
            expression->print(dst, 0);
        }
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        if (expression == NULL)
        {
            // Passing in 0 arguments auto assigns a value
            context.add_enum_value(identifier);
        }
        else
        {
            // Assume expression is a number
            // Downcast to number and evaluate
            int value = dynamic_cast<Expression*>(expression)->evaluate();
            context.add_enum_value(identifier, value);
        }
    }

private:
    std::string identifier;
    Node* expression;
};


#endif  /* ast_enum_value_hpp */

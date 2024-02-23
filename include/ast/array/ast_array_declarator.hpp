#ifndef ast_array_declarator_hpp
#define ast_array_declarator_hpp

#include "../ast_node.hpp"

/*
 *  Node for array declaration (e.g. "x[8];")
*/
class ArrayDeclarator : public Node
{
public:
    ArrayDeclarator(
        NodePtr _direct_declarator,
        NodePtr _array_size
    ) :
        direct_declarator(_direct_declarator),
        array_size(_array_size)
    {}

    virtual ~ArrayDeclarator()
    {
        delete direct_declarator;
        delete array_size;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        direct_declarator->print(dst, 0); // x
        dst << "[";
        if (array_size != NULL)
        {
            array_size->print(dst, 0); // 8
        }
        dst << "]";
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("ArrayDeclarator::evaluate() not implemented");
    }

    virtual std::string get_id() const override
    {
        return direct_declarator->get_id();
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');

        /*
            If we are defining function parameters, e.g. f(int x[]), we
            need to interpret this as a pointer instead of an array.

            i.e., `int x[]` is equivalent to `int *x` in function parameters
        */

        if (context.mode == Context::Mode::FUNCTION_DEFINITION)
        {
            context.is_pointer = true;
            direct_declarator->gen_asm(dst, dest_reg, context);
            context.is_pointer = false;
        }
        else
        {
            direct_declarator->gen_asm(dst, dest_reg, context);
        }

        Types type = direct_declarator->get_type(context);
        int arr_size;
        if (context.mode == Context::Mode::FUNCTION_DEFINITION)
        {
            // Interpret this as a pointer
            arr_size = 1;
        }
        else
        {
            arr_size = array_size->evaluate(context);

            // I moved this inside the else statement due to duplicated stack
            // allocation, but I'm not sure if this is correct

            std::string id = direct_declarator->get_id();
            int stack_loc = context.allocate_array_stack(type, arr_size, id);
        }
    }

private:
    // direct_declarator '[' constant_expression ']'
    // x [ 8 ]
    NodePtr direct_declarator;
    NodePtr array_size; // constant_expression
};

#endif // ast_array_declarator_hpp

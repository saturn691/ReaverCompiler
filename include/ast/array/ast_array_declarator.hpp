#ifndef ast_array_declarator_hpp
#define ast_array_declarator_hpp

#include "../ast_node.hpp"

/*
 *  Node for array declaration (e.g. "x[8];")
*/
class ArrayDeclarator : public Declarator
{
public:
    ArrayDeclarator(
        Declarator* _direct_declarator,
        Expression* _array_size
    ) :
        direct_declarator(_direct_declarator),
        array_size(_array_size)
    {}

    virtual ~ArrayDeclarator()
    {
        delete direct_declarator;
        delete array_size;
    }

    void print(std::ostream &dst, int indent_level) const override
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

    std::string get_id() const override
    {
        return direct_declarator->get_id();
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        int arr_size;

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
            arr_size = 1;
        }
        else
        {
            direct_declarator->gen_asm(dst, dest_reg, context);
            // TODO Maybe don't expression is a number? It works however.
            // Downcast to number and evaluate
            arr_size = dynamic_cast<Number*>(array_size)->evaluate();
            std::string id = direct_declarator->get_id();
            Types type = context.get_type(id);
            int _ = context.allocate_array_stack(type, arr_size, id);
        }

        return;
    }

private:
    // direct_declarator '[' constant_expression ']'
    // x [ 8 ]
    Declarator* direct_declarator;
    Expression* array_size; // constant_expression
};

#endif // ast_array_declarator_hpp

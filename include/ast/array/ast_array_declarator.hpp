#ifndef ast_array_declarator_hpp
#define ast_array_declarator_hpp

#include <cmath>
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
        // TODO Maybe don't expression is a number? It works however.
        // Downcast to number and evaluate
        int arr_size;
        Expression* num = dynamic_cast<Expression*>(array_size);

        Types type = context.current_declaration_type->get_type();
        std::string id = direct_declarator->get_id();

        if (context.mode_stack.top() == Context::Mode::GLOBAL_DECLARATION)
        {
            unsigned int size = context.type_size_map.at(type);
            arr_size = num->evaluate();
            unsigned int total_size = size * arr_size;
            unsigned int log_size = log2(total_size);

            // DO NOT call direct_declarator->gen_asm() here
            // Let the compiler know the existence of the variable
            dst << AST_INDENT << ".globl " << id << std::endl;
            dst << AST_INDENT << ".section .sdata, \"aw\"" << std::endl;
            dst << AST_INDENT << ".align " << log_size << std::endl;
            dst << AST_INDENT << ".type " << id << ", @object" << std::endl;
            dst << AST_INDENT << ".size " << id << ", " << total_size << std::endl;

            context.allocate_stack(type, id, true, true, {arr_size});
        }

        /*
            If we are defining function parameters, e.g. f(int x[]), we
            need to interpret this as a pointer instead of an array.

            i.e., `int x[]` is equivalent to `int *x` in function parameters
        */

        else if (context.mode_stack.top() == Context::Mode::FUNCTION_DEFINITION)
        {
            if (num)
            {
                arr_size = num->evaluate();
            }
            else
            {
                arr_size = -1;
            }

            // 100% a pointer, it is passed through the function
            context.is_array = true;
            context.array_dimensions.push_back(arr_size);
            context.is_pointer = true;

            direct_declarator->gen_asm(dst, dest_reg, context);

            context.is_array = false;
            context.is_pointer = false;
            context.array_dimensions.clear();
        }
        else
        {
            arr_size = num->evaluate();

            context.array_dimensions.push_back(arr_size);
            direct_declarator->gen_asm(dst, dest_reg, context);

            // Only allocate stack once: e.g. int x[2][2]
            if (dynamic_cast<Identifier*>(direct_declarator))
            {
                Types type = context.get_type(id);

                // NOT a pointer - lives on the stack
                int stack_loc = context.allocate_stack(
                    type, id, false, true, context.array_dimensions);

                if (context.mode_stack.top() == Context::Mode::LOCAL_DECLARATION)
                {
                    // Remember the location of the base pointer
                    dst << AST_INDENT << "addi " << dest_reg << ", " <<
                        "s0, " << stack_loc << std::endl;
                }
            }

            context.array_dimensions.clear();
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

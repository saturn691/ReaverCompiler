#ifndef AST_INIT_DECLARATOR
#define AST_INIT_DECLARATOR

#include "ast_declarator.hpp"
#include "ast_expression.hpp"
#include "pointer/ast_pointer_declarator.hpp"
#include "array/ast_array_initializer_list.hpp"

class InitDeclarator : public Declarator
{
public:
    InitDeclarator(
        Declarator* _declarator,
        Node* _initializer
    ) :
        declarator(_declarator),
        initializer(_initializer)
    {}

    virtual ~InitDeclarator()
    {
        delete declarator;
        delete initializer;
    }

    std::string get_id() const override
    {
        return declarator->get_id();
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        declarator->print(dst, indent_level);
        if (initializer)
        {
            dst << " = ";
            initializer->print(dst, indent_level);
        }
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override
    {
        // Assign some stack space to the variable
        // This will put the information into the context
        declarator->gen_asm(dst, dest_reg, context);

        // Must occur after declarator->gen_asm()
        std::string id = declarator->get_id();

        // Initialise it with the value
        // If it's a PointerDeclarator, then it's an INT type
        Types type;
        if (dynamic_cast<PointerDeclarator*>(declarator))
        {
            type = Types::INT;
        }
        else
        {
            type = context.current_declaration_type->get_type();
        }

        // Global variables are handled differently
        if (context.mode_stack.top() == Context::Mode::GLOBAL_DECLARATION)
        {
            // Initialise
            dst << id << ":" << std::endl;
            // Reg does not matter
            initializer->gen_asm(dst, dest_reg, context);
        }
        else
        {
            std::string temp_reg = context.allocate_register(dst, type, {});

            if (dynamic_cast<ArrayInitializerList*>(initializer))
            {
                // dest_reg has the base pointer address
                initializer->gen_asm(dst, dest_reg, context);
            }
            else
            {
                // Allocate stack
                // This mode must occur AFTER the declarator->gen_asm()
                context.mode_stack.push(Context::Mode::INIT_DECLARATION);
                initializer->gen_asm(dst, temp_reg, context);
                // Store the value in the stack
                context.mode_stack.pop();
                context.store(dst, temp_reg, id, type);
            }

            context.deallocate_register(dst, temp_reg);
        }
    }
private:
    Declarator* declarator;
    Node* initializer;
};


#endif /* AST_INIT_DECLARATOR */

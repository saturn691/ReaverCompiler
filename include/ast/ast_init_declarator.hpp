#ifndef AST_INIT_DECLARATOR
#define AST_INIT_DECLARATOR

#include "ast_declarator.hpp"
#include "ast_expression.hpp"
#include "pointer/ast_pointer_declarator.hpp"


class InitDeclarator : public Declarator
{
public:
    InitDeclarator(
        Declarator* _declarator,
        Expression* _initializer
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
            type = context.get_type(id);
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

            // This mode must occur AFTER the declarator->gen_asm()
            context.mode_stack.push(Context::Mode::INIT_DECLARATION);
            initializer->gen_asm(dst, temp_reg, context);
            context.mode_stack.pop();

            // Store the value in the stack
            context.store(dst, temp_reg, id, type);

            context.deallocate_register(dst, temp_reg);
        }
    }
private:
    Declarator* declarator;
    Expression* initializer;
};


#endif /* AST_INIT_DECLARATOR */

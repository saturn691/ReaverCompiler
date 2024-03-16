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
        std::string id  = declarator->get_id();

        // Assign some stack space to the variable
        // This will put the information into the context
        declarator->gen_asm(dst, dest_reg, context);

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

        std::string temp_reg = context.allocate_register(type);
        initializer->gen_asm(dst, temp_reg, context);

        // Store the value in the stack
        int stack_loc = context.get_stack_location(id);
        std::string store = context.get_store_instruction(type);
        dst << AST_INDENT << store << " " << temp_reg
            << ", " << stack_loc << "(s0)" << std::endl;

        context.deallocate_register(temp_reg);
    }
private:
    Declarator* declarator;
    Expression* initializer;
};


#endif /* AST_INIT_DECLARATOR */

#ifndef ast_function_declarator_hpp
#define ast_function_declarator_hpp

#include "../ast_node.hpp"
#include "../ast_declarator.hpp"


/*
 *  Node for defining function arguments (e.g. "f(int x, int y)")
*/
class FunctionDeclarator : public Declarator
{
public:
    // No arguments provided into the function definition
    FunctionDeclarator(
        Declarator* _direct_declarator,
        NodeList* _identifier_list
    ) :
        direct_declarator(_direct_declarator),
        identifier_list(_identifier_list)
    {}

    virtual ~FunctionDeclarator()
    {
        delete direct_declarator;
        delete identifier_list;
    }

    std::string get_id() const override
    {
        return direct_declarator->get_id();
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        direct_declarator->print(dst, 0);
        dst << "(";
        if (identifier_list)
        {
            identifier_list->print(dst, 0);
        }
        dst << ")";
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        // Let the compiler know that this function exists
        context.add_function_declaration(get_id());

        if (context.mode_stack.top() == Context::Mode::FUNCTION_DEFINITION)
        {
            dst << get_id() << ":" << std::endl;
            context.init_stack(dst);

            // Put parameters on the stack
            if (identifier_list)
            {
                identifier_list->gen_asm(dst, dest_reg, context);
            }
        }
    }

private:
    Declarator* direct_declarator;
    NodeList* identifier_list;
};


#endif  /* ast_function_declarator_hpp */

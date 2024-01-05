#ifndef ast_function_declarator_hpp
#define ast_function_declarator_hpp

#include "../ast_node.hpp"


/*
 *  Node for defining function arguments (e.g. "f(int x, int y)")
*/
class FunctionDeclarator : public Node
{
public:
    // No arguments provided into the function definition
    FunctionDeclarator(
        NodePtr _direct_declarator,
        NodePtr _identifier_list
    ) :
        direct_declarator(_direct_declarator),
        identifier_list(_identifier_list)
    {}

    virtual ~FunctionDeclarator()
    {
        delete direct_declarator;
        delete identifier_list;
    }

    virtual std::string get_id() const override
    {
        return direct_declarator->get_id();
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        direct_declarator->print(dst, 0);
        dst << "(";
        if (identifier_list)
        {
            identifier_list->print(dst, 0);
        }
        dst << ")";
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("FunctionDeclarator::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        // Let the compiler know that this function exists
        context.add_function_declaration(get_id());

        // Ok, if you're reading this, please don't question this line
        // This is to prevent declarations from generating assembly
        if (dest_reg == "MAGIC CODE")
        {
            dst << get_id() << ":" << std::endl;
            context.init_stack(dst);

            // Put parameters on the stack
            if (identifier_list)
            {
                // Pass the magic code onwards
                identifier_list->gen_asm(dst, dest_reg, context);
            }
        }
    }

private:
    NodePtr direct_declarator;
    NodePtr identifier_list;
};


#endif  /* ast_function_declarator_hpp */

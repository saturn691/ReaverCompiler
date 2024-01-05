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
        // Ok, if you're reading this, please don't question this line
        // This is to prevent declarations from generating assembly
        if (dest_reg == "MAGIC CODE")
        {
            dst << get_id() << ":" << std::endl;
            context.init_stack(dst);

            // This doesn't really matter, registers will be allocated by
            // Context &context
            std::string input_reg = "";

            // Put parameters on the stack
            if (identifier_list)
            {
                // Here we use dest_reg to pass the register of the parameters
                identifier_list->gen_asm(dst, input_reg, context);
            }
        }
    }

private:
    NodePtr direct_declarator;
    NodePtr identifier_list;
};


#endif  /* ast_function_declarator_hpp */

#ifndef ast_function_parameter_hpp
#define ast_function_parameter_hpp

#include "../ast_node.hpp"


/*
 *  Node for defining function parameters
 *  Example: "int x" in f(int x) { return x; }
*/
class FunctionParameter : public Node
{
public:
    // No arguments provided into the function definition
    FunctionParameter(
        Node* _declaration_specifiers,
        Node* _declarator
    ) :
        declaration_specifiers(_declaration_specifiers),
        declarator(_declarator)
    {}

    virtual ~FunctionParameter()
    {
        delete declaration_specifiers;
        delete declarator;
    }

    virtual std::string get_id() const override
    {
        return declarator->get_id();
    }

    virtual Types get_type(Context &context) const override
    {
        return declaration_specifiers->get_type(context);
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        declaration_specifiers->print(dst, 0);
        dst << " ";
        declarator->print(dst, 0);
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        Types type = get_type(context);
        std::string id = get_id();
        context.add_function_declaration_type(type);

        if (dest_reg == "MAGIC CODE")
        {
            context.mode = Context::Mode::FUNCTION_DEFINITION;
            std::string indent(AST_PRINT_INDENT_SPACES, ' ');
            std::string arg_reg = context.allocate_arg_register(type);
            int stack_loc = context.allocate_stack(type, id);

            declarator->gen_asm(dst, arg_reg, context);
            context.mode = Context::Mode::GLOBAL;
        }
    }

private:
    Node* declaration_specifiers;
    Node* declarator;
};


#endif  /* ast_function_parameter_hpp */

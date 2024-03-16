#ifndef ast_function_parameter_hpp
#define ast_function_parameter_hpp

#include "../ast_node.hpp"
#include "../type/ast_type.hpp"
#include "../ast_declarator.hpp"


/*
 *  Node for defining function parameters
 *  Example: "int x" in f(int x) { return x; }
*/
class FunctionParameter : public Node
{
public:
    // No arguments provided into the function definition
    FunctionParameter(
        Type* _declaration_specifiers,
        Declarator* _declarator
    ) :
        declaration_specifiers(_declaration_specifiers),
        declarator(_declarator)
    {}

    virtual ~FunctionParameter()
    {
        delete declaration_specifiers;
        delete declarator;
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        declaration_specifiers->print(dst, 0);
        dst << " ";
        declarator->print(dst, 0);
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        Types type = declaration_specifiers->get_type();
        std::string id = declarator->get_id();
        context.add_function_declaration_type(type);

        std::string arg_reg = context.allocate_arg_register(type);
        int _ = context.allocate_stack(type, id);

        declarator->gen_asm(dst, arg_reg, context);
    }

private:
    Type* declaration_specifiers;
    Declarator* declarator;
};


#endif  /* ast_function_parameter_hpp */

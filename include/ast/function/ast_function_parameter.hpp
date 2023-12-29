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
        NodePtr _declaration_specifiers,
        NodePtr _declarator
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

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        declaration_specifiers->print(dst, 0);
        dst << " ";
        declarator->print(dst, 0);
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("FunctionParameter::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        // TODO consider other types
        int stack_loc = context.allocate_stack(4, get_id());

        dst << indent << "sw " << dest_reg << ", "
            << stack_loc << "(s0)" << std::endl;
    }

private:
    NodePtr declaration_specifiers;
    NodePtr declarator;
};


#endif  /* ast_function_parameter_hpp */

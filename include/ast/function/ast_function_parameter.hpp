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

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("FunctionParameter::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        Types type = get_type(context);
        std::string id = get_id();
        context.add_function_declaration_type(id, type);

        if (dest_reg == "MAGIC CODE")
        {
            std::string indent(AST_PRINT_INDENT_SPACES, ' ');
            int stack_loc = context.allocate_stack(type, id);
            std::string arg_reg = context.allocate_arg_register(type);

            switch (type)
            {
                case Types::INT:
                case Types::UNSIGNED_INT:
                    dst << indent << "sw " << arg_reg << ", "
                        << stack_loc << "(s0)" << std::endl;
                    break;

                case Types::FLOAT:
                    dst << indent << "fsw " << arg_reg << ", "
                        << stack_loc << "(s0)" << std::endl;
                    break;

                case Types::DOUBLE:
                    // Not in RISC-V cheatsheet but in specification
                    dst << indent << "fsd " << arg_reg << ", "
                        << stack_loc << "(s0)" << std::endl;
                    break;

                default:
                    throw std::runtime_error(
                        "FunctionParameter::gen_asm() not implemented"
                    );
            }
        }
    }

private:
    NodePtr declaration_specifiers;
    NodePtr declarator;
};


#endif  /* ast_function_parameter_hpp */

#ifndef ast_function_parameter_hpp
#define ast_function_parameter_hpp

#include "../ast_node.hpp"
#include "../type/ast_type.hpp"
#include "../type/struct/ast_struct_type.hpp"
#include "../ast_declarator.hpp"


/*
 *  Node for defining function parameters
 *  Example: "int x" in f(int x) { return x; }
 *  Used for function DEFINITIONS
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

    /**
     *  For full detail on implementation, refer to section 18.2 of the RISC-V
     *  specification (titled RVG calling convention).
     *
     *  Essentiatlly this figures out where it has been passed in.
    */
    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        Types type = declaration_specifiers->get_type();
        std::string id = declarator->get_id();
        context.add_function_declaration_type(type);

        // Remember the function parameter type
        context.current_sub_declaration_type = declaration_specifiers;

        if (type == Types::STRUCT)
        {
            // TODO there is a cleaner way to do this. Do you wanna try?

            // Get the struct ID
            const StructType* struct_type
                = dynamic_cast<StructType*>(declaration_specifiers);
            std::string struct_id = struct_type->get_id();

            // Now this node has the correct members, we can allocate the stack
            struct_type = dynamic_cast<const StructType*>(
                context.struct_map[struct_id]);

            struct_type->function_parameter_gen_asm(dst, context, id);
        }
        // Normal operation
        else
        {
            std::string arg_reg = context.allocate_arg_register(type, id);

            // If the argument is a register, then we need to store it
            if (arg_reg[0] == 'a' || arg_reg[0] == 'f')
            {
                {
                    declarator->gen_asm(dst, arg_reg, context);
                }
            }
        }
    }

private:
    Type* declaration_specifiers;
    Declarator* declarator;
};


#endif  /* ast_function_parameter_hpp */

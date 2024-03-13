#ifndef ast_identifier_hpp
#define ast_identifier_hpp

#include "../ast_node.hpp"


/*
 *  Leaf node for variables / function names.
 *  Example: ["x" in "int x = 10;"] OR ["f" in "int f(int x) { return 5;}"]
*/
class Identifier : public Node
{
public:
    Identifier(std::string _id) : id(_id) {}

    virtual ~Identifier()
    {}

    void print(std::ostream &dst, int indent_level) const override
    {
        dst << id;
    }

    /**
     * Returns the identifier associated with the node.
    */
    virtual std::string get_id() const
    {
        return id;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        Types type = context.get_type(id);

        // Check if it's the variable is an enum
        int enum_value = context.get_enum_value(id);
        if (enum_value != -1)
        {
            dst << indent << "li " << dest_reg << ", "
                << enum_value << std::endl;

            return;
        }

        // Find the id on the stack - will throw exception if not found.
        int stack_loc = context.get_stack_location(id);

        // Mode 1: LOAD
        if (context.mode != Context::Mode::FUNCTION_DEFINITION)
        {
            if (context.is_pointer || context.get_is_pointer(id))
            {
                type = Types::INT;
            }
            std::string load = Context::get_load_instruction(type);
            dst << indent << load << " " << dest_reg << ", "
                << stack_loc << "(s0)" << std::endl;
        }
        // Mode 2: STORE
        else
        {
            if (context.is_pointer)
            {
                type = Types::INT;
                context.set_is_pointer(id, true);
            }
            std::string store = Context::get_store_instruction(type);
            dst << indent << store << " " << dest_reg << ", "
                << stack_loc << "(s0)" << std::endl;
        }
    }

private:
    std::string id;
};


#endif  /* ast_identifier_hpp */

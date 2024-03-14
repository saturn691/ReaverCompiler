#ifndef ast_identifier_hpp
#define ast_identifier_hpp

#include "../ast_node.hpp"
#include "../ast_declarator.hpp"

/*
 *  Leaf node for variables / function names.
 *  Example: ["x" in "int x = 10;"] OR ["f" in "int f(int x) { return 5;}"]
*/
class Identifier : public Expression
{
public:
    Identifier(const std::string &_id) : id(_id) {}

    void print(std::ostream &dst, int indent_level) const override
    {
        dst << id;
    }

    Types get_type(Context &context) const override
    {
        return context.get_type(id);
    }

    std::string get_id() const
    {
        return id;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        Types type;
        int stack_loc;
        std::string store, load;
        unsigned int size;

        // Check if it's the variable is an enum
        int enum_value = context.get_enum_value(id);
        if (enum_value != -1)
        {
            dst << indent << "li " << dest_reg << ", "
                << enum_value << std::endl;

            return;
        }

        // Find the id on the stack - will throw exception if not found.
        switch (context.mode)
        {
            case Context::Mode::DECLARATION:
                context.current_declaration_type->allocate_stack(context, id);
                break;

            // SIZEOF
            case Context::Mode::SIZEOF:
                type = context.current_declaration_type->get_type();
                size = context.get_size(id);
                dst << indent << "li " << dest_reg << ", " << size << std::endl;
                break;

            case Context::Mode::STRUCT:
                context.struct_members.emplace_back(
                    id,
                    context.current_sub_declaration_type
                );
                break;

            // STORE
            case Context::Mode::FUNCTION_DEFINITION:
                stack_loc = context.get_stack_location(id);
                type = context.get_type(id);
                if (context.is_pointer)
                {
                    type = Types::INT;
                    context.set_is_pointer(id, true);
                }
                store = Context::get_store_instruction(type);
                dst << indent << store << " " << dest_reg << ", "
                    << stack_loc << "(s0)" << std::endl;
                break;

            // Mode 1: LOAD
            default:
                if (context.is_pointer || context.get_is_pointer(id))
                {
                    type = Types::INT;
                }
                else
                {
                    type = context.get_type(id);
                }
                stack_loc = context.get_stack_location(id);
                load = Context::get_load_instruction(type);
                dst << indent << load << " " << dest_reg << ", "
                    << stack_loc << "(s0)" << std::endl;
                break;
        }

        return;
    }

private:
    std::string id;
};


#endif  /* ast_identifier_hpp */

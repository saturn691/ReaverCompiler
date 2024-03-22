#ifndef ast_struct_type_hpp
#define ast_struct_type_hpp

#include "../ast_type.hpp"
#include <vector>


/*
 *  Defines an instance of a struct
 *  (e.g. "struct x y;")
*/
class StructType : public Type
{
public:
    StructType(
        std::string _identifier
    ) :
        identifier(_identifier)
    {}

    StructType(
        std::string _identifier,
        std::vector<std::pair<std::string, TypePtr>> _members
    ) :
        identifier(_identifier),
        members(_members)
    {}

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES * indent_level), ' ');
        dst << AST_INDENT << "struct " << identifier;
        // Intentionally no std::endl
    }

    std::string get_id() const
    {
        return identifier;
    }

    Types get_type() const override
    {
        return Types::STRUCT;
    }

    unsigned int get_size(Context &context) const override
    {
        unsigned size = 0;

        for (auto member : members)
        {
            size += member.second->get_size(context);
        }

        return size;
    }

    void allocate_stack(
        Context &context,
        std::string id
    ) const override {
        // Allocate space for each member
        for (auto member : members)
        {
            std::string new_id = id + "." + member.first;
            Types type = member.second->get_type();
            if (type == Types::STRUCT)
            {
                // Recurse
                member.second->allocate_stack(context, new_id);
            }
            else
            {
                // Allocate space for the member
                context.allocate_stack(type, new_id);
            }
        }
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        context.current_declaration_type = context.struct_map[identifier];
    }

    /**
     *  TODO there is a cleaner way to do this? Do you wanna try?
     *
     *  Essentially handles everything with respect to a function definition.
     *  Checks whether it should be passed in a register or on the stack.
     *  Handles all of the context problems.
     *
     *  For full details, refer to the RVG calling convention in the RISC-V spec.
    */
    void function_parameter_gen_asm(
        std::ostream &dst,
        Context &context,
        std::string id
    ) const {
        context.id_to_struct[id] = this;
        bool invert = (get_size(context) > AST_ARG_MAX_SIZE);
        std::string temp_reg;

        // By convention, if the sizeof(struct) <= 8, it is passed through
        // a0-a7, otherwise it is passed through the stack.
        if (invert)
        {
            // There is only arg register here - it's a pointer
            std::string arg_reg = context.allocate_arg_register(Types::INT, id);
            temp_reg = context.allocate_register(dst, Types::INT, {arg_reg});

            // Move the address into a temporary register
            dst << AST_INDENT << "mv " << temp_reg << ", "
                << arg_reg << std::endl;
        }

        for (const auto &member : members)
        {
            std::string new_id = id + "." + member.first;
            Types type = member.second->get_type();
            std::string store = context.get_store_instruction(type);
            if (type == Types::STRUCT)
            {
                // TODO Recurse (warning: untested code)
                const StructType* st
                    = dynamic_cast<const StructType*>(member.second);
                st->function_parameter_gen_asm(dst, context, new_id);
            }
            else
            {
                if (invert)
                {
                    // Allocate space for the member
                    // Remember its location, but don't actually store it
                    context.allocate_bottom_stack(type, new_id);
                }
                else
                {
                    // Allocate space for the member
                    std::string arg_reg = context.allocate_arg_register(type);
                    int stack_loc = context.allocate_stack(type, new_id);

                    // Store the argument in the stack (same as Identifier)
                    dst << AST_INDENT << store << " " << arg_reg << ", "
                        << stack_loc << "(s0)" << std::endl;
                }
            }
        }

        if (invert)
        {
            context.deallocate_register(dst, temp_reg);
        }

        return;
    }


    /**
     *  TODO this code is a set it and forget it.
     *  I seriously don't know a way to make this any better. @GTA when you're
     *  looking at this, please give me some suggestions
     *
     *  Anyways, for full details see the RVG calling convention (18.2).
     *  Essentially the same thing as function_parameter_gen_asm, but inverted
    */
    void function_call_gen_asm(
        std::ostream &dst,
        Context &context,
        std::string id
    ) const {
        bool invert = (get_size(context) > AST_ARG_MAX_SIZE);
        std::string temp_reg, arg_reg;
        int bottom_stack_loc;

        // By convention, if the sizeof(struct) <= 8, it is passed through
        // a0-a7, otherwise it is passed through the stack.
        if (invert)
        {
            // There is only arg register here - it's a pointer
            arg_reg = context.allocate_arg_register(Types::INT, id);
        }

        for (const auto &member : members)
        {
            std::string new_id = id + "." + member.first;
            Types type = member.second->get_type();

            if (type == Types::STRUCT)
            {
                // TODO Recurse (warning: untested code)
                const StructType* st
                    = dynamic_cast<const StructType*>(member.second);
                st->function_call_gen_asm(dst, context, new_id);
            }
            else
            {
                std::string load = context.get_load_instruction(type);
                std::string store = context.get_store_instruction(type);

                if (invert)
                {
                    // Allocate space for the member
                    // Remember its location, but don't actually store it
                    int new_loc = context.allocate_bottom_stack(type, new_id);
                    if (member.first == members.back().first)
                    {
                        bottom_stack_loc = new_loc;
                    }

                    temp_reg = context.allocate_register(
                        dst, type, {arg_reg});

                    // Doesn't matter which register we use to load
                    // As long as it's the right type :(
                    context.load(dst, temp_reg, new_id, type);

                    // Now store it to its new location
                    dst << AST_INDENT << store << " " << temp_reg << ", "
                        << new_loc << "(sp)" << std::endl;

                    context.deallocate_register(dst, temp_reg);
                }
                else
                {
                    // Allocate a new arg_reg
                    std::string arg_reg = context.allocate_arg_register(type);

                    // Load it into the argument register ready for use
                    context.load(dst, arg_reg, new_id, type);
                }
            }
        }

        if (invert)
        {
            // Pass the address of the struct as the first argument
            dst << AST_INDENT << "addi " << arg_reg << ", sp, "
                << bottom_stack_loc << std::endl;

            context.deallocate_register(dst, arg_reg);
        }

        return;
    }

private:
    std::string identifier;
    std::vector<std::pair<std::string, TypePtr>> members;
};


#endif  /* ast_struct_type_hpp */

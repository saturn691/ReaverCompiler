#ifndef ast_struct_definition_hpp
#define ast_struct_definition_hpp

#include "../../ast_node.hpp"
#include "ast_struct_type.hpp"

/*
 *  Defines a struct
 *  (e.g. "struct x { int a; int b; };")
*/
class StructDefinition : public Type
{
public:
    StructDefinition(
        std::string _identifier,
        Node* _struct_declaration_list
    ) :
        identifier(_identifier),
        struct_declaration_list(_struct_declaration_list)
    {}

    void print(std::ostream &dst, int indent_level) const override
    {
        dst << "struct " << identifier << std::endl;
        dst << "{" << std::endl;
        struct_declaration_list->print(dst, 1);
        dst << "};" << std::endl;
        dst << std::endl;
    }

    Types get_type() const override
    {
        throw std::runtime_error("StructDefinition does not have a type");
    }

    unsigned int get_size(Context &context) const override
    {
        throw std::runtime_error("StructDefinition does not have a size");
    }

    void allocate_stack(Context &context, std::string id) const override {}

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        context.mode_stack.push(Context::Mode::STRUCT);

        // Clear the cache of struct members
        context.struct_members.clear();

        // Populate context.struct_members
        struct_declaration_list->gen_asm(dst, dest_reg, context);

        // Create a new struct type and add it to the struct map
        context.struct_map[identifier] = new StructType(
            identifier,
            context.struct_members
        );

        context.mode_stack.pop();
    }

private:
    std::string identifier;
    Node* struct_declaration_list;
};


#endif  /* ast_struct_definition_hpp */

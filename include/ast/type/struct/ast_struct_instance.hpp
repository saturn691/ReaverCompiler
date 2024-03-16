#ifndef ast_struct_instance_hpp
#define ast_struct_instance_hpp

#include "../../ast_node.hpp"


/*
 *  Defines an instance of a struct
 *  (e.g. "struct x y;")
*/
class StructInstance : public Node
{
public:
    StructInstance(std::string _identifier) : identifier(_identifier) {}

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES * indent_level), ' ');
        dst << AST_INDENT << "struct " << identifier;
        // Intentionally no std::endl
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        context.current_declaration_type = context.struct_map[identifier];
    }

private:
    std::string identifier;
};


#endif  /* ast_struct_instance_hpp */

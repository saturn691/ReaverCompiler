#ifndef ast_struct_declaration_hpp
#define ast_struct_declaration_hpp

#include "../../ast_node.hpp"


/*
 *  Struct declarations (NOT declarator)
 *  Declarations are HIGHER than declarators in the AST
 *  (e.g. "int x, y, z;")
*/
class StructDeclaration : public Node
{
public:
    StructDeclaration(
        Type* specifier_qualifier_list,
        NodeList* _struct_declarator_list
    ) :
        specifier_qualifier_list(specifier_qualifier_list),
        struct_declarator_list(_struct_declarator_list)
    {}

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES * indent_level), ' ');
        dst << indent;
        specifier_qualifier_list->print(dst, 0);
        dst << " ";
        struct_declarator_list->print(dst, 0);
        dst << ";" << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {        context.current_sub_declaration_type = specifier_qualifier_list;
        struct_declarator_list->gen_asm(dst, dest_reg, context);
    }

private:
    Type* specifier_qualifier_list;
    NodeList* struct_declarator_list;
};


#endif  /* ast_struct_declaration_hpp */

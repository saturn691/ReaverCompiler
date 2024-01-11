#ifndef ast_struct_definition_hpp
#define ast_struct_definition_hpp

#include "../../ast_node.hpp"


/*
 *  Defines a struct
 *  (e.g. "struct x { int a; int b; };")
*/
class StructDefinition : public Node
{
public:
    StructDefinition(
        std::string _identifier,
        NodePtr _struct_declaration_list
    ) :
        identifier(_identifier),
        struct_declaration_list(_struct_declaration_list)
    {}

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        dst << "struct " << identifier << std::endl;
        dst << "{" << std::endl;
        struct_declaration_list->print(dst, 1);
        dst << "};" << std::endl;
        dst << std::endl;
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        throw std::runtime_error("StructDeclaration::gen_asm() not implemented");
    }

private:
    std::string identifier;
    NodePtr struct_declaration_list;
};


#endif  /* ast_struct_definition_hpp */

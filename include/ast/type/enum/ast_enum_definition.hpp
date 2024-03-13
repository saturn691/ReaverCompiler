#ifndef ast_enum_definition_hpp
#define ast_enum_definition_hpp

#include "../../ast_node.hpp"
#include "../../ast_context.hpp"


/*
 *  Defines an enum
 *  (e.g. "enum x { a, b, c };")
*/
class EnumDefinition : public Node
{
public:
    EnumDefinition(
        Node* _enumerator_list
    ) :
        identifier(""),
        enumerator_list(_enumerator_list)
    {}

    EnumDefinition(
        std::string _identifier,
        Node* _enumerator_list
    ) :
        identifier(_identifier),
        enumerator_list(_enumerator_list)
    {}

    void print(std::ostream &dst, int indent_level) const override
    {
        dst << "enum " << identifier << std::endl;
        dst << "{";
        // Intentionally no std::endl
        enumerator_list->print(dst, 1);
        // Now there is one, this is to do with commas
        dst << std::endl << "};" << std::endl;
        dst << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        enumerator_list->gen_asm(dst, dest_reg, context);
        context.add_enum(identifier);
    }

private:
    std::string identifier;
    Node* enumerator_list;
};


#endif  /* ast_enum_definition_hpp */

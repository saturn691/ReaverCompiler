#ifndef ast_pointer_declarator_hpp
#define ast_pointer_declarator_hpp

#include "../ast_node.hpp"
#include "../type/ast_basic_type.hpp"


/*
 *  Node for pointer declaration (e.g. "*x;")
*/
class PointerDeclarator : public Declarator
{
public:
    PointerDeclarator(
        Node* _pointer,
        Declarator* _direct_declarator
    ) :
        pointer(_pointer),
        direct_declarator(_direct_declarator)
    {}

    virtual ~PointerDeclarator()
    {
        delete pointer;
        delete direct_declarator;
    }

    std::string get_id() const override
    {
        return direct_declarator->get_id();
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        if (pointer != NULL)
        {
            pointer->print(dst, 0);
        }
        if (direct_declarator != NULL)
        {
            direct_declarator->print(dst, 0);
        }
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        // tell identifier to reserve 4 bytes and ALWAYS 4 bytes
        context.is_pointer = true;
        direct_declarator->gen_asm(dst, dest_reg, context);
        context.is_pointer = false;
    }

private:
    Node* pointer;
    Declarator* direct_declarator;
};

#endif // ast_pointer_declarator_hpp

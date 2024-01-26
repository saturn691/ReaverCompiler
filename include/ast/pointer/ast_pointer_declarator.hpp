#ifndef ast_pointer_declarator_hpp
#define ast_pointer_declarator_hpp

#include "../ast_node.hpp"
#include "../type/ast_basic_type.hpp"


/*
 *  Node for pointer declaration (e.g. "*x;")
*/
class PointerDeclarator : public Node
{
public:
    PointerDeclarator(
        NodePtr _pointer,
        NodePtr _direct_declarator
    ) :
        pointer(_pointer),
        direct_declarator(_direct_declarator)
    {}

    virtual ~PointerDeclarator()
    {
        delete pointer;
        delete direct_declarator;
    }

    virtual std::string get_id() const override
    {
        return direct_declarator->get_id();
    }

    virtual Types get_type(Context &context) const override
    {
        // A pointer is always 4 bytes
        return Types::INT;
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("PointerDeclarator::evaluate() not implemented");
    }

    virtual void print(std::ostream &dst, int indent_level) const override
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

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');

        // tell identifier to reserve 4 bytes and ALWAYS 4 bytes
        context.current_declaration_type = new BasicType(Types::INT);
        direct_declarator->gen_asm(dst, dest_reg, context);
        context.set_is_pointer(true, get_id());

        delete context.current_declaration_type;
    }

private:
    NodePtr pointer;
    NodePtr direct_declarator;
};

#endif // ast_pointer_declarator_hpp

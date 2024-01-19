#ifndef ast_pointer_declarator_hpp
#define ast_pointer_declarator_hpp

#include "../ast_node.hpp"

/*
 *  Node for pointer declaration (e.g. "int* x;")
*/
class PointerDeclarator : public Node
{
public:
    PointerDeclarator(
        NodePtr _pointer,
        NodePtr _direct_declarator,
        NodePtr _direct_abstract_declarator

    ) :
        pointer(_pointer),
        direct_declarator(_direct_declarator),
        direct_abstract_declarator(_direct_abstract_declarator)
    {}

    virtual ~PointerDeclarator()
    {
        delete pointer;
        delete direct_declarator;
        delete direct_abstract_declarator;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        if (pointer != NULL)
        {
            pointer->print(dst, 0); // *
        }
        if (direct_declarator != NULL)
        {
            direct_declarator->print(dst, 0); // x
        }
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("PointerDeclarator::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        throw std::runtime_error("PointerDeclarator::gen_asm() not implemented");

    }

private:
    NodePtr pointer; // *
    NodePtr direct_declarator; // something like void foo(int *)
    NodePtr direct_abstract_declarator; // something like void foo(int *x)
};

#endif // ast_pointer_declarator_hpp

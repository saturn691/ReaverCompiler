#ifndef ast_pointer_hpp
#define ast_pointer_hpp

#include "../ast_node.hpp"
#include "../ast_context.hpp"

#include <cmath>

/*
 *  Node for pointer (e.g. "*")
*/
class Pointer : public Node
{
public:
    Pointer(
        NodePtr _pointer
    ) :
        pointer(_pointer)
    {}

    virtual ~Pointer()
    {
        delete pointer;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "*";
        if (pointer != NULL)
        {
            pointer->print(dst, 0);
        }
    }

    virtual std::string get_id() const override
    {
        throw std::runtime_error("Pointer::get_id() not implemented");
    }

    virtual Types get_type(Context &context) const override
    {
        throw std::runtime_error("Pointer::get_type() not implemented");
        // return pointer->get_type(context);
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("Pointer::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
    }

private:
    NodePtr pointer;
};

#endif // ast_pointer_hpp

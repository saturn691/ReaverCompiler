#ifndef ast_pointer_access_hpp
#define ast_pointer_access_hpp

#include "../ast_node.hpp"
#include "../ast_context.hpp"

#include <cmath>

/*
 *  Node for pointer access (e.g. "*ptr;")
*/
class PointerAccess : public Node
{
public:
    PointerAccess(
        NodePtr _pointer
    ) :
        pointer(_pointer)
    {}

    virtual ~PointerAccess()
    {
        delete pointer;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "*";
        pointer->print(dst, 0);
    }

    virtual std::string get_id() const override
    {
        return pointer->get_id();
    }

    virtual Types get_type(Context &context) const override
    {
        return pointer->get_type(context);
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("PointerAccess::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        throw std::runtime_error("PointerAccess::gen_asm() not implemented");
        // std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        // Types type = pointer->get_type(context);
        // std::string reg = context.allocate_register(type);
        // std::string id = pointer->get_id();

        // pointer->gen_asm(dst, reg, context);

        // dst << indent << "lw " << dest_reg << ", 0(" << reg << ")" << std::endl;
    }

private:
    // '*' postfix_expression
    NodePtr pointer;
};

#endif // ast_pointer_access_hpp

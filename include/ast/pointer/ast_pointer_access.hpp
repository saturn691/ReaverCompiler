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
        NodePtr _type_qualifier_list,
        NodePtr _pointer
    ) :
        type_qualifier_list(_type_qualifier_list),
        pointer(_pointer)
    {}

    virtual ~PointerAccess()
    {
        delete type_qualifier_list;
        delete pointer;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "*";
        if (type_qualifier_list != NULL)
        {
            type_qualifier_list->print(dst, 0);
        }
        if (pointer != NULL)
        {
            pointer->print(dst, 0);
        }
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
    }

private:
    NodePtr type_qualifier_list;
    NodePtr pointer;
};

#endif // ast_pointer_access_hpp

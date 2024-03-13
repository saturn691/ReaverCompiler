#ifndef ast_sizeof_hpp
#define ast_sizeof_hpp

#include "../ast_node.hpp"
#include "../ast_context.hpp"

/*
 *  Defines the sizeof operator (e.g. "sizeof(int)")
*/
class SizeOf : public Node
{
public:
    SizeOf(Node* _child) : child(_child) {}

    ~SizeOf()
    {
        delete child;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES * indent_level), ' ');

        dst << indent << "sizeof(";
        child->print(dst, 0);
        dst << ")" << std::endl;
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');

        unsigned int size = child->get_size(context);
        dst << indent << "li " << dest_reg << ", " << size << std::endl;
    }

private:
    // unary_expression or type_name
    Node* child;
};


#endif  /* ast_sizeof_hpp */

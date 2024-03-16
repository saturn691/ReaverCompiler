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
        Node* _pointer
    ) :
        pointer(_pointer)
    {}

    virtual ~Pointer()
    {
        delete pointer;
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "*";
        if (pointer != NULL)
        {
            pointer->print(dst, 0);
        }
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
    }

private:
    Node* pointer;
};

#endif // ast_pointer_hpp

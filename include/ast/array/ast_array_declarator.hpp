#ifndef ast_array_declarator_hpp
#define ast_array_declarator_hpp

#include "../ast_node.hpp"

/*
 *  Node for array declaration (e.g. "x[8];")
*/
class ArrayDeclarator : public Node
{
public:
    ArrayDeclarator(
        Node* _direct_declarator,
        Node* _array_size
    ) :
        direct_declarator(_direct_declarator),
        array_size(_array_size)
    {}

    virtual ~ArrayDeclarator()
    {
        delete direct_declarator;
        delete array_size;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        direct_declarator->print(dst, 0); // x
        dst << "[";
        array_size->print(dst, 0); // 8
        dst << "]";
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');

        direct_declarator->gen_asm(dst, dest_reg, context);

        Types type = direct_declarator->get_type(context);
        int arr_size = array_size->evaluate(context);
        std::string id = direct_declarator->get_id();
        int stack_loc = context.allocate_array_stack(type, arr_size, id);
    }

private:
    // direct_declarator '[' constant_expression ']'
    // x [ 8 ]
    Node* direct_declarator;
    Node* array_size; // constant_expression
};

#endif // ast_array_declarator_hpp

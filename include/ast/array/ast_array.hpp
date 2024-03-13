#ifndef ast_array_declaration_hpp
#define ast_array_declaration_hpp

#include "../ast_node.hpp"

/*
 *  Node for array declaration (e.g. "int arr[5];")
*/
class ArrayDeclaration : public Node
{
public:
    ArrayDeclaration(
        Node* _declaration_specifier,
        Node* _declarator,
        Node* _array_size
    ) :
        declaration_specifier(_declaration_specifier),
        declarator(_declarator),
        array_size(_array_size)
    {}

    virtual ~ArrayDeclaration()
    {
        delete declaration_specifier;
        delete declarator;
        delete array_size;
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        declaration_specifier->print(dst, 0);
        dst << " ";
        declarator->print(dst, 0);
        dst << "[";
        array_size->print(dst, 0);
        dst << "]";
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        throw std::runtime_error("ArrayDeclaration::gen_asm() not implemented");
    }

private:
    Node* declaration_specifier;
    Node* declarator;
    Node* array_size;
};

#endif // ast_array_declaration_hpp

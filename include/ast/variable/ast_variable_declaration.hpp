#ifndef ast_variable_declaration_hpp
#define ast_variable_declaration_hpp

#include "../ast_node.hpp"


/*
 *  Node for variable declarations (e.g. "int x;")
 *  Currently cannot support multiple types, (e.g. "float x");
 *  Currently cannot support multiple declarations (e.g. "int x, y;")
*/
class VariableDeclaration : public Node
{
public:
    VariableDeclaration(
        NodePtr _declaration_specifiers,
        NodePtr _init_declarator_list
    ) :
        declaration_specifiers(_declaration_specifiers),
        init_declarator_list(_init_declarator_list)
    {}

    virtual ~VariableDeclaration()
    {}

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES * indent_level), ' ');

        dst << indent;
        declaration_specifiers->print(dst, 0); // int
        dst << " ";
        init_declarator_list->print(dst, 0); // x (supposed to be able to support multiple declarations)
        dst << ";" << std::endl;
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("Return::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        // Reserve space on the stack
        // TODO deal with multiple declarations
        context.allocate_stack(
            get_type(context),
            init_declarator_list->get_id()
        );

        init_declarator_list->gen_asm(dst, dest_reg, context);
    }

private:
    NodePtr declaration_specifiers;
    NodePtr init_declarator_list;
};


#endif  /* ast_variable_declaration_hpp */

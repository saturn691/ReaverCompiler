#ifndef ast_declaration_hpp
#define ast_declaration_hpp

#include "../ast_node.hpp"


/*
 *  Node for declarations
 *  (e.g. "int x;")
 *
 *  Can be used for both variables and function declarations
 *  (e.g. "int f(int x)").
 */
class Declaration : public Node
{
public:
    Declaration(
        Type* _declaration_specifiers,
        Node* _init_declarator_list
    ) :
        declaration_specifiers(_declaration_specifiers),
        init_declarator_list(_init_declarator_list)
    {}

    virtual ~Declaration()
    {}

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES * indent_level), ' ');

        dst << indent;
        declaration_specifiers->print(dst, 0); // int
        dst << " ";
        init_declarator_list->print(dst, 0); // x (supposed to be able to support multiple declarations)
        dst << ";" << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        // Pass information about the type, down the AST tree
        context.current_declaration_type = declaration_specifiers;
        context.current_declaration_type->gen_asm(dst, dest_reg, context);

        if (context.mode_stack.top() == Context::Mode::GLOBAL)
        {
            context.mode_stack.push(Context::Mode::GLOBAL_DECLARATION);
        }
        else
        {
            context.mode_stack.push(Context::Mode::LOCAL_DECLARATION);
        }

        // Will be an identifier list
        init_declarator_list->gen_asm(dst, dest_reg, context);
        context.mode_stack.pop();
    }

private:
    Type* declaration_specifiers;
    Node* init_declarator_list;
};


#endif  /* ast_declaration_hpp */

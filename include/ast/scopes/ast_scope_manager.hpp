#ifndef ast_scope_manager_hpp
#define ast_scope_manager_hpp

#include "../ast_node.hpp"


/*
 *  Node for managing the scope of a function.
*/
class ScopeManager : public Node
{
public:
    // No arguments provided into the function definition
    ScopeManager(
        NodeList* _statement_list,
        NodeList* _declaration_list
    ) :
        statement_list(_statement_list),
        declaration_list(_declaration_list)
    {}

    virtual ~ScopeManager()
    {
        delete statement_list;
        delete declaration_list;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "{" << std::endl;
        if (statement_list != NULL)
        {
            statement_list->print(dst, indent_level);
        }
        dst << std::endl;
        if (declaration_list != NULL)
        {
            declaration_list->print(dst, indent_level);
        }
        dst << "}" << std::endl;
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');

        context.push_identifier_map();

        if (statement_list != NULL)
        {
            statement_list->gen_asm(dst, dest_reg, context);
        }
        if (declaration_list != NULL)
        {
            declaration_list->gen_asm(dst, dest_reg, context);
        }

        context.pop_identifier_map();

    }

private:
    NodeList* statement_list;
    NodeList* declaration_list;
};


#endif  /* ast_scope_manager_hpp */

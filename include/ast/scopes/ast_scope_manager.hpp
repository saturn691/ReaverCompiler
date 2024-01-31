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
        NodePtr _statement_list,
        NodePtr _declaration_list
    ) :
        statement_list(_statement_list),
        declaration_list(_declaration_list)
    {}

    virtual ~ScopeManager()
    {
        delete statement_list;
        delete declaration_list;
    }

    virtual std::string get_id() const override
    {
        throw std::runtime_error("ScopeManager::get_id() not implemented");
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "{" << std::endl;
        statement_list->print(dst, indent_level);
        dst << std::endl;
        declaration_list->print(dst, indent_level);
        dst << "}" << std::endl;
    }

    virtual Types get_type(Context& context) const override
    {
        throw std::runtime_error("ScopeManager::get_type() not implemented");
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("ScopeManager::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        throw std::runtime_error("ScopeManager::gen_asm() not implemented");

    }

private:
    NodePtr statement_list;
    NodePtr declaration_list;
};


#endif  /* ast_scope_manager_hpp */

#ifndef ast_function_definition_hpp
#define ast_function_definition_hpp

#include "../ast_node.hpp"


/*
 *  Node for defining functions (e.g. "int f() { return 5; }")
*/
class FunctionDefinition : public Node
{
public:
    // No arguments provided into the function definition
    FunctionDefinition(
        NodePtr _declaration_specifier,
        NodePtr _declarator,
        NodePtr _compound_statement
    ) :
        declaration_specifier(_declaration_specifier),
        declarator(_declarator),
        compound_statement(_compound_statement)
    {}

    // Arguments provided into the function definition
    FunctionDefinition(
        NodePtr _declaration_specifier,
        NodePtr _declarator,
        NodePtr _declaration_list,
        NodePtr _compound_statement
    ) :
        declaration_specifier(_declaration_specifier),
        declarator(_declarator),
        declaration_list(_declaration_list),
        compound_statement(_compound_statement)
    {}

    virtual ~FunctionDefinition()
    {
        delete declaration_specifier;
        delete declarator;
        delete declaration_list;
        delete compound_statement;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        declaration_specifier->print(dst, 0);
        dst << " ";
        declarator->print(dst, 0);
        dst << "(";
        if (declaration_list)
        {
            declaration_list->print(dst, 0);
        }
        dst << ")" << std::endl;

        dst << "{" << std::endl;
        compound_statement->print(dst, indent_level + 1);
        dst << "}";
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("FunctionDefinition::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string dest_reg,
        Context &context
    ) const override {
        std::string id = declarator->get_id();

        // Header section
        dst << ".text" << std::endl;
        dst << ".globl " << id << std::endl;
        dst << std::endl;

        // Body section
        dst << id << ":" << std::endl;
        compound_statement->gen_asm(dst, dest_reg, context);
    }

private:
    NodePtr declaration_specifier;
    NodePtr declarator;
    NodePtr declaration_list;
    NodePtr compound_statement;
};


#endif  /* ast_function_definition_hpp */

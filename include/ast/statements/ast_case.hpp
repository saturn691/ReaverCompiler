#ifndef ast_case_hpp
#define ast_case_hpp

#include "../ast_node.hpp"
#include "../ast_context.hpp"
#include <sstream>


/*
 *  Node for case in switch-case statements
 *  (e.g. "case 1:" in "switch (x) { case 1: return 1; }")
*/
class Case : public Node
{
public:
    Case(
        Node* _expression,
        Node* _statement
    ) :
        expression(_expression),
        statement(_statement)
    {}

    virtual ~Case()
    {
        delete expression;
        delete statement;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "case (";
        expression->print(dst, 0);
        dst << "):" << std::endl;
        statement->print(dst, indent_level + 1);
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        std::string case_label = context.get_unique_label("switch_case");
        std::string case_reg = context.allocate_register(
            expression->get_type(context)
        );

        // Redirect into stringstream to be outputted later
        std::stringstream &expression_ss = context.switch_cases.first;
        std::stringstream &statement_ss = context.switch_cases.second;

        expression->gen_asm(expression_ss, case_reg, context);
        expression_ss << indent << "beq " << context.switch_reg
            << ", " << case_reg << ", "
            << case_label << std::endl;

        statement_ss << case_label << ":" << std::endl;
        statement->gen_asm(statement_ss, dest_reg, context);

        context.deallocate_register(case_reg);
    }

private:
    Node* expression;
    Node* statement;
};


class DefaultCase : public Node
{
public:
    DefaultCase(
        Node* _statement
    ) :
        statement(_statement)
    {}

    virtual ~DefaultCase()
    {
        delete statement;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "default" << std::endl;
        dst << indent << ":" << std::endl;
        statement->print(dst, indent_level + 1);
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        std::string default_label = context.get_unique_label("switch_default");

        // Redirect into stringstream to be outputted later
        std::stringstream &expression_ss = context.switch_default.first;
        std::stringstream &statement_ss = context.switch_default.second;

        expression_ss << indent << "j " << default_label << std::endl;

        statement_ss << default_label << ":" << std::endl;
        statement->gen_asm(statement_ss, dest_reg, context);
    }

private:
    Node* statement;
};


#endif  /* ast_case_hpp */

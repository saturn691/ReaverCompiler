#ifndef AST_TERNARY_HPP
#define AST_TERNARY_HPP

#include "../ast_expression.hpp"
#include "../ast_context.hpp"
#include "ast_if_else.hpp"


/*
 *  Node for ternary (e.g. "x < 5 ? 1 : 0").
 *  Placed as a separate node for better pretty printing
*/
class Ternary : public Expression
{
public:
    /**
     * We don't have access to C++ pointers (flex only allows C pointers), so
     * transfer control of the deletion of the if_else object to the Ternary.
     * Still need the pointers for the "overwritten" print() function
    */
    Ternary(
        Expression* _condition,
        Expression* _then_statement,
        Expression* _else_statement
    ) :
        condition(_condition),
        then_statement(_then_statement),
        else_statement(_else_statement)
    {
        if_else = new IfElse(_condition, _then_statement, _else_statement);
    }

    virtual ~Ternary()
    {
        delete if_else;
    }

    std::string get_id() const override
    {
        throw std::runtime_error("Ternary does not have an id");
    }

    Types get_type(Context &context) const override
    {
        return then_statement->get_type(context);
    }


    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "(";
        condition->print(dst, 0); // condition
        dst << ") ? (";
        then_statement->print(dst, 0); // then statement
        dst << ") : (";
        else_statement->print(dst, 0); // else statement
        dst << ")" << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        // Use the if-else code generation
        if_else->gen_asm(dst, dest_reg, context);
    }

private:
    Expression* condition;
    Expression* then_statement;
    Expression* else_statement;
    IfElse* if_else;
};

#endif // AST_TERNARY_HPP

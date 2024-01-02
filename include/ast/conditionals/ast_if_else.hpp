#ifndef ast_if_else_hpp
#define ast_if_else_hpp

#include "../ast_node.hpp"


/*
 *  Node for if-else (e.g. "if (x < 5) { ... } else { ... }")
*/
class IfElse : public Node
{
public:
    IfElse(
        NodePtr _expression,
        NodePtr _then_statement,
        NodePtr _else_statement = nullptr
    ) :
        expression(_expression),
        then_statement(_then_statement),
        else_statement(_else_statement)
    {}

    virtual ~IfElse(){
        delete expression;
        delete then_statement;
        delete else_statement;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "if" << std::endl;
        dst << indent << "(" << std::endl;
        expression->print(dst, indent_level + 1); // condition
        dst << indent << ")" << std::endl;
        dst << indent << "{" << std::endl;
        then_statement->print(dst, indent_level + 1); // then statement
        dst << indent << "}" << std::endl;
        if (else_statement != nullptr) {
            dst << indent << "else" << std::endl;
            dst << indent << "{" << std::endl;
            else_statement->print(dst, indent_level + 1); // else statement
            dst << indent << "}" << std::endl;
        }
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("IfElse::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        // TODO: fetch the condition, then, else code
        // if (condition_code) then (then_code) else (else_code)
        // if (condition) { then } else { else }

        std::string indent(AST_PRINT_INDENT_SPACES, ' ');

        expression->gen_asm(dst, dest_reg, context);
        if (else_statement != nullptr) {
            dst << indent << "beq a0, zero, else" << std::endl;
        } else {
            dst << indent << "beq a0, zero, end" << std::endl;
        }

        dst << "then:" << std::endl; // doesn't make a difference
        then_statement->gen_asm(dst, dest_reg, context);
        dst << indent << "j end" << std::endl;

        if (else_statement != nullptr) {
            dst << "else:" << std::endl;
            else_statement->gen_asm(dst, dest_reg, context);
            // dst << indent << "j end" << std::endl;
        }

        dst << "end:" << std::endl;
    }

private:
    NodePtr expression;
    NodePtr then_statement;
    NodePtr else_statement;
};


#endif  /* ast_if_else_hpp */

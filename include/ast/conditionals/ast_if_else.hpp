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
        Node* _condition,
        Node* _then_statement,
        Node* _else_statement = nullptr
    ) :
        condition(_condition),
        then_statement(_then_statement),
        else_statement(_else_statement)
    {}

    virtual ~IfElse(){
        delete condition;
        delete then_statement;
        delete else_statement;
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "if" << std::endl;
        dst << AST_INDENT << "(" << std::endl;
        condition->print(dst, indent_level + 1); // condition
        dst << AST_INDENT << ")" << std::endl;
        dst << AST_INDENT << "{" << std::endl;
        then_statement->print(dst, indent_level + 1); // then statement
        dst << AST_INDENT << "}" << std::endl;
        if (else_statement != nullptr) {
            dst << AST_INDENT << "else" << std::endl;
            dst << AST_INDENT << "{" << std::endl;
            else_statement->print(dst, indent_level + 1); // else statement
            dst << AST_INDENT << "}" << std::endl;
        }
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        // if (condition_code) then (then_code) else (else_code)
        // if (condition) { then } else { else }

        std::string else_label = context.get_unique_label("if");
        std::string end_label = context.get_unique_label("if");

        std::string condition_reg = context.allocate_register(
            dst, Types::INT, {dest_reg});

        condition->gen_asm(dst, condition_reg, context);

        if (else_statement != nullptr)
        {
            dst << AST_INDENT << "beq " << condition_reg
                << ", zero, " << else_label << std::endl;
        }
        else
        {
            dst << AST_INDENT << "beq " << condition_reg
                << ", zero, " << end_label << std::endl;
        }

        context.deallocate_register(dst, condition_reg);

        then_statement->gen_asm(dst, dest_reg, context);
        dst << AST_INDENT << "j " << end_label << std::endl;

        if (else_statement != nullptr)
        {
            dst << else_label << ":" << std::endl;
            else_statement->gen_asm(dst, dest_reg, context);
        }

        dst << end_label << ":" << std::endl;
    }

private:
    Node* condition;
    Node* then_statement;
    Node* else_statement;
};


#endif  /* ast_if_else_hpp */

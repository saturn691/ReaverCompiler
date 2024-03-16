#ifndef AST_MODULO_HPP
#define AST_MODULO_HPP

#include "ast_operator.hpp"


/*
 *  Node for modulo
*/
class Modulo : public Operator
{
public:
    using Operator::Operator;

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES* indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        dst << " % ";
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {

    }

private:
    const std::unordered_map<Types, std::string> ins_map = {
        {Types::UNSIGNED_CHAR, "mul"},
        {Types::CHAR, "mul"},
        {Types::UNSIGNED_SHORT, "mul"},
        {Types::SHORT, "mul"},
        {Types::INT, "mul"},
        {Types::UNSIGNED_INT, "mul"},
        {Types::LONG, "mul"},
        {Types::UNSIGNED_LONG, "mul"},
        {Types::FLOAT, "fmul.s"},
        {Types::DOUBLE, "fmul.d"},
        {Types::LONG_DOUBLE, "fmul.d"}
    };

};


#endif  /* AST_MODULO_HPP */

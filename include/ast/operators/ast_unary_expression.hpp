#ifndef ast_unary_expression_hpp
#define ast_unary_expression_hpp

enum class UnaryOperator
{
    ADDRESS,
    DEREFERENCE,
    ADD,
    SUB,
    LOGICAL_NOT,
    BITWISE_NOT,
};


#include "../ast_expression.hpp"
#include "../ast_context.hpp"




/*
 *  Node for unary expressions
 *  (e.g. "+x", "-x", "~x", "!x", "&x")
*/
class UnaryExpression : public Expression
{
public:
    UnaryExpression(
        UnaryOperator _unary_operator,
        Expression* _cast_expression
    ) :
        unary_operator(_unary_operator),
        cast_expression(_cast_expression)
    {}

    virtual ~UnaryExpression();

    void print(std::ostream &dst, int indent_level) const override;

    std::string get_id() const override;

    Types get_type(Context &context) const override;

    UnaryOperator get_unary_operator() const;

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override;

private:
    UnaryOperator unary_operator;
    Expression* cast_expression;

    const std::unordered_map<UnaryOperator, std::string> unary_operator_map = {
        {UnaryOperator::ADDRESS, "&"},
        {UnaryOperator::DEREFERENCE, "*"},
        {UnaryOperator::ADD, "+"},
        {UnaryOperator::SUB, "-"},
        {UnaryOperator::LOGICAL_NOT, "!"},
        {UnaryOperator::BITWISE_NOT, "~"},
    };

    const std::unordered_map<Types, std::string> negate_map = {
        {Types::UNSIGNED_CHAR, "neg"},
        {Types::CHAR, "neg"},
        {Types::UNSIGNED_SHORT, "neg"},
        {Types::SHORT, "neg"},
        {Types::UNSIGNED_INT, "neg"},
        {Types::INT, "neg"},
        {Types::UNSIGNED_LONG, "neg"},
        {Types::LONG, "neg"},
        {Types::FLOAT, "fneg.s"},
        {Types::DOUBLE, "fneg.d"},
        {Types::LONG_DOUBLE, "fneg.d"}
    };
};


#endif  /* ast_unary_expression_hpp */

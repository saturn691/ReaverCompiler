#include "ast/operators/ast_unary_expression.hpp"


UnaryExpression::~UnaryExpression()
{
    delete cast_expression;
}

void UnaryExpression::print(std::ostream &dst, int indent_level) const
{
    std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

    dst << AST_INDENT << unary_operator_map.at(unary_operator);
    cast_expression->print(dst, 0);
}

std::string UnaryExpression::get_id() const
{
    return cast_expression->get_id();
}

Types UnaryExpression::get_type(Context &context) const
{
    return cast_expression->get_type(context);
}

UnaryOperator UnaryExpression::get_unary_operator() const
{
    return unary_operator;
}

void UnaryExpression::gen_asm(
    std::ostream &dst,
    std::string &dest_reg,
    Context &context
) const {
    std::string id, load;
    int address;

    if (context.mode_stack.top() == Context::Mode::GLOBAL_DECLARATION)
    {
        context.unary_operator = unary_operator;
        cast_expression->gen_asm(dst, dest_reg, context);
        return;
    }

    Context::Mode mode = context.mode_stack.top();
    context.mode_stack.push(Context::Mode::OPERATOR);

    if (unary_operator == UnaryOperator::DEREFERENCE)
    {
        context.is_pointer = true;
    }

    cast_expression->gen_asm(dst, dest_reg, context);
    switch (unary_operator)
    {
        case UnaryOperator::ADDRESS:
            // TODO pass this onto cast_expression->gen_asm()
            id = cast_expression->get_id();
            address = context.get_stack_location(id);
            dst << AST_INDENT << "addi " << dest_reg
                << ", s0, " << address << std::endl;
            context.is_pointer = true;
            break;

        case UnaryOperator::DEREFERENCE:
            // for pointers -> dereference
            if (mode != Context::Mode::ASSIGN)
            {
                load = Context::get_load_instruction(get_type(context));
                dst << AST_INDENT << load << " " << dest_reg
                    << ", 0(" << dest_reg << ")" << std::endl;
            }
            break;

        case UnaryOperator::ADD:
            break;

        case UnaryOperator::SUB:
            dst << AST_INDENT << "neg " << dest_reg << ", "
                << dest_reg << std::endl;
            break;

        case UnaryOperator::LOGICAL_NOT:
            dst << AST_INDENT << "seqz " << dest_reg << ", "
                << dest_reg << std::endl;
            break;

        case UnaryOperator::BITWISE_NOT:
            dst << AST_INDENT << "not " << dest_reg
                << ", " << dest_reg << std::endl;
            break;

    }

    context.mode_stack.pop();
    context.is_pointer = false;
}

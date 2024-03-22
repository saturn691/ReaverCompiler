#ifndef ast_assign_hpp
#define ast_assign_hpp

#include "../../ast_node.hpp"
#include "../../ast_context.hpp"
#include "./../../array/ast_array_access.hpp"
#include "../ast_operator.hpp"
#include "../ast_unary_expression.hpp"

/*
 *  Node for assignment (e.g. "x = 5;")
*/
class Assign : public Expression
{
public:
    Assign(
        Expression* _unary_expression,
        AssignOp* _assignment_operator,
        Expression* _assignment_expression
    ) :
        unary_expression(_unary_expression),
        assignment_operator(_assignment_operator),
        assignment_expression(_assignment_expression)
    {}

    std::string get_id() const override
    {
        return unary_expression->get_id();
    }

    Types get_type(Context &context) const override
    {
        return unary_expression->get_type(context);
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES * indent_level), ' ');

        dst << indent;
        unary_expression->print(dst, 0); // x[0]
        dst << " ";
        assignment_operator->print(dst, 0); // =
        dst << " ";
        assignment_expression->print(dst, 0); // 5
        dst << ";" << std::endl;
    }

    // TODO clean up this function
    /**
     *  dest_reg has to be used here as this is apparently valid syntax:
     *
     *  a = (b = 5).
     *  This means b = 5 is evaluated, so a = 5.
    */
    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        context.mode_stack.push(Context::Mode::ASSIGN);

        std::string id = unary_expression->get_id();
        Types type = context.get_type(id);

        ArrayAccess* array_access = dynamic_cast<ArrayAccess*>(unary_expression);
        UnaryExpression* unary_expr = dynamic_cast<UnaryExpression*>(unary_expression);

        // Start of assembly generation
        std::string temp_reg = context.allocate_register(dst, type, {dest_reg});

        // If the types do not match we need to use the temp_reg
        if (type != context.get_type("!" + dest_reg))
        {
            unary_expression->gen_asm(dst, temp_reg, context);
            Operator::move_reg(
                dst,
                temp_reg,
                dest_reg,
                type,
                context.get_type("!" + dest_reg)
            );
        }
        else
        {
            unary_expression->gen_asm(dst, dest_reg, context);
        }

        // Put the assignment expression into a temporary register
        std::string reg = context.allocate_register(dst, type, {dest_reg});
        std::string store = Context::get_store_instruction(type);

        gen_assignment_asm(dst, reg, context);

        /*
            Pointer dereference

            For situations like:
                int x = 5;
                int *y = &x;
                *y = 10;
            where the assignment is a pointer dereference, we need to
            dereference the pointer. Similar to pointer element access in
            array access, we need to dereference the pointer before we can
            assign to it. As such, we need to check if the unary expression is
            a pointer dereference. Since this cannot be done in the pointer hpp
            files, it has to be done here to align with the node calls of the
            AST.
        */
        if (array_access)
        {
            context.store(dst, reg, id, type, dest_reg);
        }
        else if (unary_expr)
        {
            UnaryOperator unary_op = unary_expr->get_unary_operator();
            if (unary_op == UnaryOperator::DEREFERENCE)
            {
                dst << AST_INDENT << store << " " << reg << ", 0("
                    << dest_reg << ")" << std::endl;
            }
        }
        else
        {
            context.store(dst, reg, id, type);

            // Move the result to the destination register
            Operator::move_reg(
                dst,
                reg,
                dest_reg,
                unary_expression->get_type(context),
                assignment_expression->get_type(context)
            );
        }

        // Restore the mode
        context.mode_stack.pop();
        context.deallocate_register(dst, temp_reg);
        context.deallocate_register(dst, reg);
    }

private:

    /**
     * Helper function to make the code in gen_asm() easier to read.
    */
    void gen_assignment_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const {
        AssignOpType a_id = assignment_operator->get_id();

        switch (a_id)
        {
            case AssignOpType::ASSIGN:
                assignment_expression->gen_asm(dst, dest_reg, context);
                break;

            default:
                Operator(
                    unary_expression,
                    assignment_expression,
                    assign_to_op.at(a_id)
                ).gen_asm(dst, dest_reg, context);
                break;
        }

        return;
    }

    const std::unordered_map<AssignOpType, OperatorType> assign_to_op = {
        {AssignOpType::MUL_ASSIGN, OperatorType::MUL},
        {AssignOpType::DIV_ASSIGN, OperatorType::DIV},
        {AssignOpType::MOD_ASSIGN, OperatorType::MOD},
        {AssignOpType::ADD_ASSIGN, OperatorType::ADD},
        {AssignOpType::SUB_ASSIGN, OperatorType::SUB},
        {AssignOpType::LEFT_ASSIGN, OperatorType::LEFT_SHIFT},
        {AssignOpType::RIGHT_ASSIGN, OperatorType::RIGHT_SHIFT},
        {AssignOpType::AND_ASSIGN, OperatorType::BITWISE_AND},
        {AssignOpType::XOR_ASSIGN, OperatorType::BITWISE_XOR},
        {AssignOpType::OR_ASSIGN, OperatorType::BITWISE_OR}
    };

    Expression* unary_expression;
    AssignOp* assignment_operator;
    Expression* assignment_expression;
};


#endif  /* ast_assign_hpp */

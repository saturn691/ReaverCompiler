#ifndef ast_assign_hpp
#define ast_assign_hpp

#include "../../ast_node.hpp"
#include "../../ast_context.hpp"
#include "../ast_add.hpp"
#include "../ast_bitwise_and.hpp"
#include "../ast_bitwise_or.hpp"
#include "../ast_bitwise_xor.hpp"
#include "../ast_subtract.hpp"
#include "../ast_multiply.hpp"
#include "../ast_divide.hpp"
#include "../ast_modulo.hpp"
#include "../ast_shift.hpp"
#include "./../../array/ast_array_access.hpp"
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
    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string id = unary_expression->get_id();
        int stack_loc = context.get_stack_location(id);
        Types type = context.get_type(id);
        unsigned int multiplier = Context::type_size_map.at(type);
        context.pointer_multiplier = context.get_is_pointer(id) ? multiplier : 1;

        ArrayAccess* array_access = dynamic_cast<ArrayAccess*>(unary_expression);
        UnaryExpression* unary_expr = dynamic_cast<UnaryExpression*>(unary_expression);

        context.mode_stack.push(Context::Mode::ASSIGN);

        // Start of assembly generation
        unary_expression->gen_asm(dst, dest_reg, context);

        // Put the assignment expression into a temporary register
        std::string reg = context.allocate_register(type);
        std::string store = Context::get_store_instruction(type);

        gen_assignment_asm(dst, reg, context);

        if (array_access)
        {
            std::string arr_reg = array_access->get_index_register();

            dst << AST_INDENT << store << " " << reg
                << ", 0(" << arr_reg << ")" << std::endl;
            context.deallocate_register(arr_reg);
        }
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
        else if (unary_expr)
        {
            std::string unary_op = unary_expr->get_unary_operator();
            if (unary_op == "*")
            {
                dst << AST_INDENT << store << " " << reg << ", 0("
                    << dest_reg << ")" << std::endl;
            }
        }
        else
        {
            dst << AST_INDENT << store << " " << reg << ", "
                << stack_loc << "(s0)" << std::endl;

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
        context.pointer_multiplier = 1;
        context.mode_stack.pop();
        context.deallocate_register(reg);
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

            case AssignOpType::MUL_ASSIGN:
                Mul(unary_expression, assignment_expression)
                    .gen_asm(dst, dest_reg, context);
                break;

            case AssignOpType::DIV_ASSIGN:
                Divide(unary_expression, assignment_expression)
                    .gen_asm(dst, dest_reg, context);
                break;

            case AssignOpType::MOD_ASSIGN:
                Modulo(unary_expression, assignment_expression)
                    .gen_asm(dst, dest_reg, context);
                break;

            case AssignOpType::ADD_ASSIGN:
                Add(unary_expression, assignment_expression)
                    .gen_asm(dst, dest_reg, context);
                break;

            case AssignOpType::SUB_ASSIGN:
                Sub(unary_expression, assignment_expression)
                    .gen_asm(dst, dest_reg, context);
                break;

            case AssignOpType::LEFT_ASSIGN:
                LeftShift(unary_expression, assignment_expression)
                    .gen_asm(dst, dest_reg, context);
                break;

            case AssignOpType::RIGHT_ASSIGN:
                RightShift(unary_expression, assignment_expression)
                    .gen_asm(dst, dest_reg, context);
                break;

            case AssignOpType::AND_ASSIGN:
                BitwiseAnd(unary_expression, assignment_expression)
                    .gen_asm(dst, dest_reg, context);
                break;

            case AssignOpType::XOR_ASSIGN:
                BitwiseXor(unary_expression, assignment_expression)
                    .gen_asm(dst, dest_reg, context);
                break;

            case AssignOpType::OR_ASSIGN:
                BitwiseOr(unary_expression, assignment_expression)
                    .gen_asm(dst, dest_reg, context);
                break;

            default:
                throw std::runtime_error(
                    "Assign::gen_asm(): Unsupported assignment operator"
                );
                break;
        }

        return;
    }

    Expression* unary_expression;
    AssignOp* assignment_operator;
    Expression* assignment_expression;
};


#endif  /* ast_assign_hpp */

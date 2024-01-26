#ifndef ast_assign_hpp
#define ast_assign_hpp

#include "../../ast_node.hpp"
#include "../../ast_context.hpp"
#include "../ast_add.hpp"
#include "./../../array/ast_array_access.hpp"
#include "../ast_unary_expression.hpp"

/*
 *  Node for assignment (e.g. "x = 5;")
*/
class Assign : public Node
{
public:
    Assign(
        NodePtr _unary_expression,
        NodePtr _assignment_operator,
        NodePtr _assignment_expression
    ) :
        unary_expression(_unary_expression),
        assignment_operator(_assignment_operator),
        assignment_expression(_assignment_expression)
    {}

    virtual std::string get_id() const override
    {
        return unary_expression->get_id();
    }

    virtual Types get_type(Context &context) const override
    {
        return unary_expression->get_type(context);
    }

    virtual void print(std::ostream &dst, int indent_level) const override
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

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("Assign::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        std::string id = assignment_operator->get_id();

        context.mode = Context::Mode::ASSIGN;

        unary_expression->gen_asm(dst, dest_reg, context);
        int stack_loc = context.get_stack_location(get_id());
        Types type = get_type(context);

        // Put the assignment expression into a temporary register
        std::string reg = context.allocate_register(type);

        unsigned int multiplier = Context::type_size_map.at(type);
        if (context.get_is_pointer(get_id()))
        {
            context.pointer_multiplier = multiplier;
        }
        else
        {
            context.pointer_multiplier = 1;
        }
        assignment_expression->gen_asm(dst, reg, context);

        const ArrayAccess* array_access;
        array_access = dynamic_cast<const ArrayAccess*>(unary_expression);
        const UnaryExpression* unary_expr;
        unary_expr = dynamic_cast<const UnaryExpression*>(unary_expression);
        std::string arr_reg;

        if (array_access)
        {
            arr_reg = array_access->get_index_register();
            switch (type)
            {
                case Types::INT:
                case Types::UNSIGNED_INT:
                    dst << indent << "sw " << reg
                        << ", 0(" << arr_reg << ")" << std::endl;
                    context.deallocate_register(arr_reg);
                    break;

                default:
                    throw std::runtime_error(
                        "Assign::gen_asm(): Unsupported type for assignment"
                    );
                    break;
            }
        }
        // Pointer dereference
        /*
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
                switch (type)
                {
                    case Types::INT:
                    case Types::UNSIGNED_INT:
                        dst << indent << "sw " << reg << ", 0("
                            << dest_reg << ")" << std::endl;
                        break;

                    default:
                        throw std::runtime_error(
                            "Assign::gen_asm(): Unsupported type for assignment"
                        );
                        break;
                }
            }
        }
        else
        {
            // TODO implement for all assignment operators
            if (id == "+=")
            {
                Add(
                    unary_expression,
                    assignment_expression
                ).gen_asm(dst, reg, context);
            }

            std::string store = Context::get_store_instruction(type);
            dst << indent << store << " " << reg << ", "
                << stack_loc << "(s0)" << std::endl;
        }

        context.mode = Context::Mode::GLOBAL; // Change mode back to default
        context.deallocate_register(reg);
        context.pointer_multiplier = 1;
    }

private:
    NodePtr unary_expression;
    NodePtr assignment_operator;
    NodePtr assignment_expression;
};


#endif  /* ast_assign_hpp */

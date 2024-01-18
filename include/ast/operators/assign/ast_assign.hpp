#ifndef ast_assign_hpp
#define ast_assign_hpp

#include "../../ast_node.hpp"
#include "../../ast_context.hpp"
#include "../ast_add.hpp"
#include "./../../array/ast_array_access.hpp"

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

        unary_expression->gen_asm(dst, dest_reg, context);
        int stack_loc = context.get_stack_location(get_id());
        Types type = get_type(context);

        // TODO consider other types
        // Put the assignment expression into a temporary register
        std::string reg = context.allocate_register(type);
        assignment_expression->gen_asm(dst, reg, context);

        const ArrayAccess* array_access = dynamic_cast<const ArrayAccess*>(unary_expression);
        std::string arr_reg;

        if (array_access)
        {
            arr_reg = array_access->get_index_register();
            switch (type)
            {
                case Types::INT:
                case Types::UNSIGNED_INT:
                    dst << indent << "sw " << reg << ", 0(" << arr_reg << ")" << std::endl;
                    context.deallocate_register(arr_reg);
                    break;

                // case Types::FLOAT:
                //     dst << indent << "fsw " << reg << ", 0(" << arr_reg << ")" << std::endl;
                //     break;

                // case Types::DOUBLE:
                //     dst << indent << "dsw " << reg << ", 0(" << arr_reg << ")" << std::endl;
                //     break;

                default:
                    throw std::runtime_error(
                        "Assign::gen_asm(): Unsupported type for assignment"
                    );
                    break;
            }

            context.deallocate_register(reg);
            return;
        }

        // TODO implement for all assignment operators
        if (id == "+=")
        {
            Add(
                unary_expression,
                assignment_expression
            ).gen_asm(dst, reg, context);
        }
        switch (type)
        {
            case Types::INT:
            case Types::UNSIGNED_INT:
                dst << indent << "sw " << reg << ", "
                    << stack_loc << "(s0)" << std::endl;
                break;

            case Types::FLOAT:
                dst << indent << "fsw " << reg << ", "
                    << stack_loc << "(s0)" << std::endl;
                break;

            case Types::DOUBLE:
                dst << indent << "dsw " << reg << ", "
                    << stack_loc << "(s0)" << std::endl;
                break;

            default:
                throw std::runtime_error(
                    "Assign::gen_asm(): Unsupported type for assignment"
                );
                break;
            }

        context.deallocate_register(reg);
    }

private:
    NodePtr unary_expression;
    NodePtr assignment_operator;
    NodePtr assignment_expression;
};


#endif  /* ast_assign_hpp */

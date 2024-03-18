#ifndef ast_function_argument_list_hpp
#define ast_function_argument_list_hpp

#include "../ast_node.hpp"
#include "../ast_expression.hpp"


/*
 *  Node for defining function argument list as part of function calls
 *  Example: "x, y, z" in f(x, y, z);
*/
class FunctionArgumentList : public NodeList
{
public:
    using NodeList::NodeList;

    void print(std::ostream &dst, int indent_level) const override
    {
        for (auto &node : nodes)
        {
            node->print(dst, indent_level);
            dst << ", ";
        }
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        context.deallocate_arg_registers();

        for (auto &node : nodes)
        {
            /*
            Note on function calls:

            The RISC-V convention for function calls involves storing the arguments
            in the function argument registers (a0-a7). Then the call can be made,
            but it is made in the ast_function_call.hpp file.
            */

            // Downcasting is safe here- check the parser.y file
            Expression* assignment_expression = dynamic_cast<Expression*>(node);

            Types type = assignment_expression->get_type(context);

            // Generate an argument register (will be a0-a7 or a stack location)
            std::string arg_reg_or_location = context.allocate_arg_register(type);
            if (arg_reg_or_location[0] == 'a' || arg_reg_or_location[0] == 'f')
            {
                assignment_expression->gen_asm(dst, arg_reg_or_location, context);
            }
            // No more registers - store in a stack location
            else
            {
                std::string temp_reg = context.allocate_register(
                    dst, type, {"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7"}
                );

                assignment_expression->gen_asm(dst, temp_reg, context);

                dst << AST_INDENT << "sw " << temp_reg << ", "
                    << arg_reg_or_location << "(sp)" << std::endl;

                context.deallocate_register(dst, temp_reg);
            }
        }
    }

};


#endif  /* ast_function_argument_list_hpp */

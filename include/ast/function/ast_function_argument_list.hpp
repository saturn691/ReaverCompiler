#ifndef ast_function_argument_list_hpp
#define ast_function_argument_list_hpp

#include "../ast_node.hpp"
#include "../ast_expression.hpp"

#include "../type/struct/ast_struct_type.hpp"


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

            /*
            TODO this
            What if we run out of space in the argument registers?
            I can only spend so much time on this compiler before the deadline.
            Plus 11 tests on this, so please don't let this be the downfall.
            */
            if (type == Types::STRUCT)
            {
                std::string id = assignment_expression->get_id();

                // Now obtain the correct node to generate assembly
                const StructType* struct_node = dynamic_cast<const StructType*>(
                    context.id_to_struct[id]);

                // Do the function call
                struct_node->function_call_gen_asm(dst, context, id);
            }
            else
            {
                // Generate an argument register
                // Will be a0-a7 or a stack location
                std::string reg_or_loc = context.allocate_arg_register(type);
                if (reg_or_loc[0] == 'a' || reg_or_loc[0] == 'f')
                {
                    assignment_expression->gen_asm(dst, reg_or_loc, context);
                }
                // No more registers - store in a stack location
                else
                {
                    std::string temp_reg = context.allocate_register(
                        dst,
                        type,
                        {"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7"}
                    );

                    assignment_expression->gen_asm(dst, temp_reg, context);

                    std::string store = context.get_store_instruction(type);
                    dst << AST_INDENT << store << " "<< temp_reg << ", "
                        << reg_or_loc << "(sp)" << std::endl;

                    context.deallocate_register(dst, temp_reg);
                }
            }
        }
    }
};


#endif  /* ast_function_argument_list_hpp */

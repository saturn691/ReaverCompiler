#ifndef ast_function_argument_hpp
#define ast_function_argument_hpp

#include "../ast_node.hpp"


/*
 *  Node for defining function arguments (e.g. "x" in "f(x, y)")
*/
class FunctionArgument : public Node
{
public:
    // No arguments provided into the function definition
    FunctionArgument(
        Node* _assignment_expression
    ) :
        assignment_expression(_assignment_expression)
    {}

    virtual ~FunctionArgument()
    {
        delete assignment_expression;
    }


    void print(std::ostream &dst, int indent_level) const override
    {
        assignment_expression->print(dst, 0);
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        /*
        Note on function calls:

        The RISC-V convention for function calls involves storing the arguments
        in the function argument registers (a0-a7). Then the call can be made,
        but it is made in the ast_function_call.hpp file.
        */

        assignment_expression->gen_asm(dst, dest_reg, context);
    }

private:
    Node* assignment_expression;
};


#endif  /* ast_function_argument_hpp */

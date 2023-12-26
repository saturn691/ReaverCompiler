#ifndef ast_assign_op_hpp
#define ast_assign_op_hpp

#include "../../ast_node.hpp"


/*
 *  Node for the assignment operator
 *  Example: =, +=, -=, >>=, <<=
*/
class AssignOp : public Node
{
public:
    AssignOp(std::string _op) : op(_op) {}

    virtual ~AssignOp()
    {}

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        dst << op;
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("AssignOp::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string dest_reg,
        Context &context
    ) const override {
        throw std::runtime_error("AssignOp::gen_asm() not implemented");
    }

private:
    std::string op;
};


#endif  /* ast_assign_op_hpp */

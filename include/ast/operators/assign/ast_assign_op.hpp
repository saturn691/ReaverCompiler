#ifndef ast_assign_op_hpp
#define ast_assign_op_hpp

#include "../../ast_node.hpp"

enum class AssignOpType
{
    ASSIGN,
    MUL_ASSIGN,
    DIV_ASSIGN,
    MOD_ASSIGN,
    ADD_ASSIGN,
    SUB_ASSIGN,
    LEFT_ASSIGN,
    RIGHT_ASSIGN,
    AND_ASSIGN,
    XOR_ASSIGN,
    OR_ASSIGN
};


/*
 *  Node for the assignment operator
 *  Example: =, +=, -=, >>=, <<=
*/
class AssignOp : public Node
{
public:
    AssignOp(AssignOpType _op) : op(_op)
    {}

    virtual ~AssignOp()
    {}

    AssignOpType get_id() const
    {
        return op;
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        dst << op_map.at(op);
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        throw std::runtime_error("AssignOp::gen_asm() not implemented");
    }

private:
    AssignOpType op;
    const std::unordered_map<AssignOpType, std::string> op_map = {
        {AssignOpType::ASSIGN, "="},
        {AssignOpType::MUL_ASSIGN, "*="},
        {AssignOpType::DIV_ASSIGN, "/="},
        {AssignOpType::MOD_ASSIGN, "%="},
        {AssignOpType::ADD_ASSIGN, "+="},
        {AssignOpType::SUB_ASSIGN, "-="},
        {AssignOpType::LEFT_ASSIGN, "<<="},
        {AssignOpType::RIGHT_ASSIGN, ">>="},
        {AssignOpType::AND_ASSIGN, "&="},
        {AssignOpType::XOR_ASSIGN, "^="},
        {AssignOpType::OR_ASSIGN, "|="}
    };
};


#endif  /* ast_assign_op_hpp */

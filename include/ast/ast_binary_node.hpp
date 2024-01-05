#ifndef ast_binary_node_hpp
#define ast_binary_node_hpp

#include "ast_node.hpp"


/*
 *  Node with left and right children
*/
class BinaryNode : public Node
{
public:
    BinaryNode(NodePtr _left, NodePtr _right) : left(_left), right(_right)
    {}

    virtual ~BinaryNode()
    {
        delete left;
        delete right;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        left->print(dst, indent_level);
        right->print(dst, indent_level);
    }

    virtual Types get_type(Context &context) const override
    {
        Types left_type = left->get_type(context);
        Types right_type = right->get_type(context);
        Types return_type;

        /*
        This logic might seem a bit strange, but it checks the left and right
        types and returns the one with the higher priority.

        e.g. INT + INT -> INT
        e.g. INT + FLOAT -> FLOAT
        e.g. UNSIGNED_INT + INT -> INT
        */

        if (left_type > right_type)
        {
            return_type = left_type;
        }
        else
        {
            return_type = right_type;
        }

        return return_type;
    }

    NodePtr get_left() const { return left; }
    NodePtr get_right() const { return right; }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("Return::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        left->gen_asm(dst, dest_reg, context);
        right->gen_asm(dst, dest_reg, context);
    }

protected:
    NodePtr left;
    NodePtr right;
};

#endif  /* ast_binary_node_hpp */

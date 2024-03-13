#ifndef ast_binary_node_hpp
#define ast_binary_node_hpp

#include "ast_node.hpp"


/*
 *  Node with left and right children
*/
class BinaryNode : public Node
{
public:
    BinaryNode() {}
    BinaryNode(Node* _left, Node* _right) : left(_left), right(_right)
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

    Node* get_left() const { return left; }
    Node* get_right() const { return right; }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        left->gen_asm(dst, dest_reg, context);
        right->gen_asm(dst, dest_reg, context);
    }

protected:
    Node* left;
    Node* right;
};

#endif  /* ast_binary_node_hpp */

#ifndef ast_operator_hpp
#define ast_operator_hpp

#include "../ast_node.hpp"


/*
 *  Base class for all operators (e.g. + - * / << >> & |)
*/
class Operator : public Node
{
public:
    Operator(NodePtr _left, NodePtr _right) : left(_left), right(_right) {}

    ~Operator()
    {
        delete left;
        delete right;
    }

    NodePtr get_left() const
    {
        return left;
    }

    NodePtr get_right() const
    {
        return right;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        throw std::runtime_error("Operator::print() not implemented");
    }

private:
    NodePtr left;
    NodePtr right;
};


#endif /* ast_operator_hpp */

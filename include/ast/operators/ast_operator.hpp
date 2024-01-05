#ifndef ast_operator_hpp
#define ast_operator_hpp

#include "../ast_node.hpp"


/*
 *  Base class for all operators (e.g. + - * / << >> & |)
*/
class Operator : public BinaryNode
{
public:
    using BinaryNode::BinaryNode;

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        throw std::runtime_error("Operator::print() not implemented");
    }
};


#endif /* ast_operator_hpp */

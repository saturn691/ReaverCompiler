#ifndef ast_node_hpp
#define ast_node_hpp

#include <string>
#include <iostream>
#include <map>
#include <memory>

#include "ast_context.hpp"


class Node;

typedef const Node *NodePtr;

class Node
{
public:
    virtual ~Node() {}

    // Tell and expression to print itself to the given stream
    virtual void print(std::ostream &dst) const = 0;

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const {
        throw std::runtime_error("Node::evaluate() not implemented");
    }

    // RISC-V asm generation
    virtual void gen_asm(
        std::ostream &dst,
        std::string dest_reg,
        Context &context
    ) const {
        throw std::runtime_error("Node::gen_asm() not implemented");
    }
};


#endif /* ast_node_hpp */
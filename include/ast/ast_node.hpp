#ifndef ast_node_hpp
#define ast_node_hpp

#include <string>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "ast_context.hpp"


class Node;

typedef const Node *NodePtr;


/*
 *  Base class for nodes in the AST. Only contains virtual functions.
*/
class Node
{
public:
    virtual ~Node()
    {}

    // Tell and expression to print itself to the given stream
    virtual void print(std::ostream &dst, int indent_level) const = 0;

    // Finds the identifier in one of the branches.
    // Not necessary to implement in every child class.
    virtual std::string get_id() const
    {
        throw std::runtime_error("Node::get_id() not implemented");
    }

    virtual double evaluate(Context &context) const
    {
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

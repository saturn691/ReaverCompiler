#ifndef ast_node_hpp
#define ast_node_hpp

#include <string>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "ast_types.hpp"

class Node;
class Context;


/**
 *  Base class for nodes in the AST. Only contains virtual functions.
 *  This is the standard interface for all nodes and only contains the functions
 *  that all nodes must implement.
*/
class Node
{
public:
    virtual ~Node()
    {}

    // Tell and expression to print itself to the given stream
    virtual void print(std::ostream &dst, int indent_level) const = 0;

    // RISC-V asm generation
    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const = 0;
};


/**
 *  This data structure (array) is much better than the data structure used
 *  previously (linked list), as it's easier to access elements and iterate
*/
class NodeList : public Node
{
public:
    NodeList() : nodes({})
    {}
    NodeList(Node *node) : nodes({node})
    {}

    ~NodeList()
    {
        for (auto &node : nodes)
        {
            delete node;
        }
    }

    void push_back(Node *node)
    {
        nodes.push_back(node);
    }

    int size() const
    {
        return nodes.size();
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        for (auto &node : nodes)
        {
            if (node != nullptr)
            {
                node->print(dst, indent_level);
            }
        }
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override
    {
        for (auto &node : nodes)
        {
            if (node != nullptr)
            {
                node->gen_asm(dst, dest_reg, context);
            }
        }
    }

protected:
    std::vector<Node*> nodes;
};


#endif /* ast_node_hpp */

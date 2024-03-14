#ifndef ast_empty_node_hpp
#define ast_empty_node_hpp

#include "ast_node.hpp"


/*
 *  The "eplison" node of the compiler, the node that does absolutely nothing
 *  The only job is to not throw an error.
*/
class EmptyNode : public Node
{
public:

    void print(std::ostream &dst, int indent_level) const override
    {
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
    }
};

#endif  /* ast_empty_node_hpp */

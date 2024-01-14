#ifndef ast_struct_declarator_list_hpp
#define ast_struct_declarator_list_hpp

#include "../../ast_binary_node.hpp"


/*
 *  Defines comma seperated struct declarations
 *  (e.g. "a, b, c")
*/
class StructDeclaratorList : public BinaryNode
{
public:
    using BinaryNode::BinaryNode;

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        get_left()->print(dst, 0);
        dst << ", ";
        get_right()->print(dst, 0);
    }
};


#endif  /* ast_struct_declarator_list_hpp */

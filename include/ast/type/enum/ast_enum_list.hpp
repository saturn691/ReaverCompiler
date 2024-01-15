#ifndef ast_enum_list_hpp
#define ast_enum_list_hpp

#include "../../ast_binary_node.hpp"


/*
 *  Defines comma seperated enum declarations with new lines
 *  (e.g. "a = 1,
 *          b,
 *          c"
 *  )
*/
class EnumList : public BinaryNode
{
public:
    using BinaryNode::BinaryNode;

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        get_left()->print(dst, 1);
        dst << ", ";
        get_right()->print(dst, 1);
    }
};


#endif  /* ast_enum_list_hpp */

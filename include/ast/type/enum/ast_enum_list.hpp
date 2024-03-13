#ifndef ast_enum_list_hpp
#define ast_enum_list_hpp

#include "../../ast_node.hpp"


/*
 *  Defines comma seperated enum declarations with new lines
 *  (e.g. "a = 1,
 *          b,
 *          c"
 *  )
*/
class EnumList : public NodeList
{
public:
    using NodeList::NodeList;
};


#endif  /* ast_enum_list_hpp */

#ifndef ast_sizeof_hpp
#define ast_sizeof_hpp

#include "../ast_node.hpp"


/*
 *  Defines the sizeof operator (e.g. "sizeof(int)")
*/
class SizeOf : public Node
{
public:

private:
    NodePtr return_node;
};


#endif  /* ast_sizeof_hpp */

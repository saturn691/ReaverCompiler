#ifndef ast_type_hpp
#define ast_type_hpp

#include "../ast_node.hpp"
#include "../ast_types.hpp"


class Type;
typedef const Type *TypePtr;

/**
 *  Base class for types (structs, unions or basic types)
*/
class Type : public Node
{
public:
    Type() {}
};


#endif  /* ast_type_hpp */

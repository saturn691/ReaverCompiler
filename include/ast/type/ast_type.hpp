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

    virtual Types get_type() const = 0;

    virtual unsigned int get_size(Context &context) const = 0;

    virtual void allocate_stack(Context &context, std::string id) const = 0;
};


#endif  /* ast_type_hpp */

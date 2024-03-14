#ifndef ast_declarator_hpp
#define ast_declarator_hpp

#include "ast_node.hpp"


/**
 *  Contains an additional get_id() function associated with
 *  the declarator
*/
class Declarator : public Node
{
public:
    Declarator() {}
    Declarator(std::string _id) : id(_id) {}

    virtual ~Declarator()
    {}

    virtual std::string get_id() const
    {
        return id;
    }

protected:
    std::string id;
};


#endif  /* ast_declarator_hpp */

#ifndef ast_struct_access_hpp
#define ast_struct_access_hpp

#include "../../ast_node.hpp"


/*
 *  Struct declarations (NOT declarator)
 *  Declarations are HIGHER than declarators in the AST
 *  (e.g. "int x, y, z;")
*/
class StructAccess : public Node
{
public:
    StructAccess(
        NodePtr _postfix_expression,
        NodePtr _identifier
    ) :
        postfix_expression(_postfix_expression),
        identifier(_identifier)
    {}

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        postfix_expression->print(dst, 0);
        dst << ".";
        identifier->print(dst, 0);
    }

private:
    NodePtr postfix_expression;
    NodePtr identifier;
};


#endif  /* ast_struct_access_hpp */

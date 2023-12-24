#ifndef ast_number_hpp
#define ast_number_hpp

#include "../ast_node.hpp"


class Number : public Node
{
public:
    Number(double _value) : value(_value) {}

    virtual ~Number()
    {}

private:
    double value;
};


#endif  /* ast_number_hpp */

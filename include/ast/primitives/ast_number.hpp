#ifndef ast_number_hpp
#define ast_number_hpp

#include "../ast_node.hpp"


/*
 *  Leaf node for numbers (e.g. "10" in "int x = 10;")
*/
class Number : public Node
{
public:
    Number(double _value) : value(_value) {}

    virtual ~Number()
    {}

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        dst << value;
    }

    virtual double evaluate(Context &context) const override
    {
        return value;
    }

    virtual void gen_asm(
        std::ostream &dst,
        int dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        dst << indent << "li x" << dest_reg << ", " << value << std::endl;
    }

private:
    double value;
};


#endif  /* ast_number_hpp */

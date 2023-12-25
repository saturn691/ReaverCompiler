#ifndef ast_number_hpp
#define ast_number_hpp

#include "../ast_node.hpp"


class Number : public Node
{
public:
    Number(double _value) : value(_value) {}

    virtual ~Number()
    {}

    virtual void print(std::ostream &dst) const override
    {
        dst << value;
    }

    virtual double evaluate(
        const std::map<std::string,double> &bindings
    ) const override {
        return value;
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string dest_reg,
        Context &context
    ) const override {
        throw std::runtime_error("Number::gen_asm() not implemented");
    }

private:
    double value;
};


#endif  /* ast_number_hpp */

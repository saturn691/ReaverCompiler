#ifndef ast_identifier_hpp
#define ast_identifier_hpp

#include "../ast_node.hpp"


/*
 *  Leaf node for variables / function names.
 *  Example: ["x" in "int x = 10;"] OR ["f" in "int f(int x) { return 5;}"]
*/
class Identifier : public Node
{
public:
    Identifier(std::string _id) : id(_id) {}

    virtual ~Identifier()
    {}

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        dst << id;
    }

    virtual std::string get_id() const override
    {
        return id;
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("Identifier::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        int dest_reg,
        Context &context
    ) const override {
        throw std::runtime_error("Identifier::gen_asm() not implemented");
    }

private:
    std::string id;
};


#endif  /* ast_identifier_hpp */

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
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        // Find the id on the stack - will throw exception if not found.
        int stack_loc = context.variable_map.at(id).stack_location;

        // TODO- deal with other types
        dst << indent << "lw " << dest_reg << ", "
            << stack_loc << "(s0)" << std::endl;
    }

private:
    std::string id;
};


#endif  /* ast_identifier_hpp */

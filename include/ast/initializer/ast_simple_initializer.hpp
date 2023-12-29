#ifndef ast_simple_initializer_hpp
#define ast_simple_initializer_hpp

#include "../ast_node.hpp"

/*
 * Node for simple variable declaration with initialization (e.g. "int x = 1234;")
 */
class SimpleInitializer : public Node
{
public:
    SimpleInitializer(
        NodePtr _declaration_specifiers,
        NodePtr _initializer
    ) :
        declaration_specifiers(_declaration_specifiers),
        initializer(_initializer)
    {}

    virtual std::string get_id() const override
    {
        return declaration_specifiers->get_id();
    }

    virtual ~SimpleInitializer()
    {}

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES * indent_level), ' ');

        dst << indent;
        declaration_specifiers->print(dst, 0); // int x
        dst << " = "; // =
        initializer->print(dst, 0); // 5
        dst << " ";
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("SimpleInitializer::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        std::string id = declaration_specifiers->get_id();
        int stack_loc = context.variable_map.at(id).stack_location;

        std::string temp_reg = context.allocate_register(Types::INT);
        initializer->gen_asm(dst, temp_reg, context);

        dst << indent << "sw " << temp_reg << ", " << stack_loc << "(s0)" << std::endl;

        context.deallocate_register(temp_reg);
    }

private:
    NodePtr declaration_specifiers;
    NodePtr initializer;
};

#endif  /* ast_simple_initializer_hpp */

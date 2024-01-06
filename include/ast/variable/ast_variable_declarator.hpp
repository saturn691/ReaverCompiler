#ifndef ast_variable_declarator_hpp
#define ast_variable_declarator_hpp

#include "../ast_node.hpp"


/*
 *  Node for variable declarators (e.g. "x" in "int x;").
*/
class VariableDeclarator : public Node
{
public:
    VariableDeclarator(
        NodePtr _identifier
    ) :
        identifier(_identifier)
    {}

    virtual ~VariableDeclarator()
    {
        delete identifier;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        identifier->print(dst, indent_level);
    }

    virtual std::string get_id() const override
    {
        return identifier->get_id();
    }

    virtual Types get_type(Context &context) const override
    {
        // Passed from the parent node of the AST
        return context.current_declaration_type;
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("VariableDeclarator::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        // Reserve space on the stack
        context.allocate_stack(get_type(context), get_id());

        identifier->gen_asm(dst, dest_reg, context);
    }

private:
    NodePtr identifier;
};


#endif  /* ast_variable_declarator_hpp */

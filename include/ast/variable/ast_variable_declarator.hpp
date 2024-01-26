#ifndef ast_variable_declarator_hpp
#define ast_variable_declarator_hpp

#include "../ast_node.hpp"
#include "../ast_context.hpp"


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
        return identifier->get_type(context);
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
        if (context.mode == Context::Mode::FUNCTION_DEFINITION)
        {
            identifier->gen_asm(dst, dest_reg, context);
        }
        // int x has a different meaning in a function or a struct
        // definition
        else if (context.mode != Context::Mode::STRUCT)
        {
            // Reserve space on the stack
            context.current_declaration_type->allocate_stack(
                context,
                get_id()
            );
        }
        else
        {
            // Add the member to the struct
            std::string id = get_id();
            context.struct_members.emplace_back(
                id,
                context.current_sub_declaration_type
            );
        }
    }

private:
    NodePtr identifier;
};


#endif  /* ast_variable_declarator_hpp */

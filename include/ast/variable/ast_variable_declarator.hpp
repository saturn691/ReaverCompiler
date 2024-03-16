#ifndef ast_variable_declarator_hpp
#define ast_variable_declarator_hpp

#include "../ast_node.hpp"
#include "../primitives/ast_identifier.hpp"
#include "../ast_context.hpp"


/*
 *  Node for variable declarators (e.g. "x" in "int x;").
*/
class VariableDeclarator : public Node
{
public:
    VariableDeclarator(
        Identifier* _identifier
    ) :
        identifier(_identifier)
    {}

    virtual ~VariableDeclarator()
    {
        delete identifier;
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        identifier->print(dst, indent_level);
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        if (context.mode_stack.top() == Context::Mode::FUNCTION_DEFINITION)
        {
            identifier->gen_asm(dst, dest_reg, context);
        }
        // int x has a different meaning in a function or a struct
        // definition
        else if (context.mode_stack.top() != Context::Mode::STRUCT)
        {
            // Reserve space on the stack
            context.current_declaration_type->allocate_stack(
                context,
                identifier->get_id()
            );
        }
        else
        {
            // Add the member to the struct
            std::string id = identifier->get_id();
            context.struct_members.emplace_back(
                id,
                context.current_sub_declaration_type
            );
        }
    }

private:
    Identifier* identifier;
};


#endif  /* ast_variable_declarator_hpp */

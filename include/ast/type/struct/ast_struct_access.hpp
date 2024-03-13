#ifndef ast_struct_access_hpp
#define ast_struct_access_hpp

#include "../../ast_node.hpp"


/*
 *  Struct declarations (NOT declarator)
 *  Declarations are HIGHER than declarators in the AST
 *  (e.g. "int x, y, z;")
*/
class StructAccess : public Expression
{
public:
    StructAccess(
        Expression* _postfix_expression,
        Identifier* _identifier
    ) :
        postfix_expression(_postfix_expression),
        identifier(_identifier)
    {}

    void print(std::ostream &dst, int indent_level) const override
    {
        postfix_expression->print(dst, 0);
        dst << ".";
        identifier->print(dst, 0);
    }

    std::string get_id() const override
    {
        return postfix_expression->get_id()
             + "."
             + identifier->get_id();
    }

    Types get_type() const override
    {
        // Find the id on the stack - will throw exception if not found
        // std::string id = get_id();
        // return context.get_type(id);
        // TODO - implement this
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        // Find the id on the stack - will throw exception if not found
        std::string id = get_id();
        int stack_loc = context.get_stack_location(id);
        Types type = get_type();

        switch (type)
        {
            case Types::INT:
            case Types::UNSIGNED_INT:
                dst << indent << "lw " << dest_reg << ", "
                    << stack_loc << "(s0)" << std::endl;
                break;

            case Types::FLOAT:
                dst << indent << "flw " << dest_reg << ", "
                    << stack_loc << "(s0)" << std::endl;
                break;

            case Types::DOUBLE:
                dst << indent << "fld " << dest_reg << ", "
                    << stack_loc << "(s0)" << std::endl;
                break;

            // TODO- deal with other types
            default:
                throw std::runtime_error("gen_asm() not implemented");
        }

    }

private:
    Expression* postfix_expression;
    Identifier* identifier;
};


#endif  /* ast_struct_access_hpp */

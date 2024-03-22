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
        Identifier* _identifier,
        bool _dereference = false
    ) :
        postfix_expression(_postfix_expression),
        identifier(_identifier),
        dereference(_dereference)
    {}

    void print(std::ostream &dst, int indent_level) const override
    {
        postfix_expression->print(dst, 0);
        dst << dereference_map.at(dereference);
        identifier->print(dst, 0);
    }

    std::string get_id() const override
    {
        // Hacky context map only accepts "." and does not support nested
        return postfix_expression->get_id()
             + "."
             + identifier->get_id();
    }

    Types get_type(Context &context) const override
    {
        // Find the id on the stack - will throw exception if not found
        std::string id = get_id();
        return context.get_type(id);
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        // Find the id on the stack - will throw exception if not found
        std::string id = get_id();
        Types type = get_type(context);

        // If the type does not match, we need to move it to the correct type
        if (!Operator::reg_type_match(dest_reg, type))
        {
            std::string temp_reg = context.allocate_register(dst, type, {dest_reg});

            context.load(dst, temp_reg, id, type);
            Operator::move_reg(dst, temp_reg, dest_reg, type,
                                context.get_type("!" + dest_reg));

            context.deallocate_register(dst, dest_reg);
        }
        // Normal operation
        else
        {
            context.load(dst, dest_reg, id, type);
        }

    }

private:
    Expression* postfix_expression;
    Identifier* identifier;
    bool dereference;

    const std::unordered_map<bool, std::string> dereference_map = {
        {true, "->"},
        {false, "."}
    };
};


#endif  /* ast_struct_access_hpp */

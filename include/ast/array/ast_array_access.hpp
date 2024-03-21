#ifndef ast_array_access_hpp
#define ast_array_access_hpp

#include "../ast_expression.hpp"
#include "../ast_context.hpp"
#include "../operators/ast_operator.hpp"

#include <cmath>

/*
 *  Node for array access (e.g. "arr[5];")
*/
class ArrayAccess : public Expression
{
public:
    ArrayAccess(
        Identifier* _identifier,
        Expression* _index
    ) :
        identifier(_identifier),
        index(_index)
    {}

    virtual ~ArrayAccess()
    {
        delete identifier;
        delete index;
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        identifier->print(dst, 0);
        dst << "[";
        index->print(dst, 0);
        dst << "]";
    }

    Types get_type(Context &context) const override
    {
        if (context.mode_stack.top() == Context::Mode::ASSIGN)
        {
            // Will return the address
            return Types::INT;
        }
        else
        {
            return identifier->get_type(context);
        }
    }

    std::string get_id() const override
    {
        return identifier->get_id();
    }

    std::string get_index_register() const
    {
        return index_reg;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string id = identifier->get_id();
        Types type = context.get_type(id);
        std::string load_ins = context.get_load_instruction(type);

        // Index must be an integer
        index_reg = context.allocate_register(dst, Types::INT, {dest_reg});
        index->gen_asm(dst, index_reg, context);

        context.load_array_address(dst, dest_reg, id, type, index_reg);

        // No need to load for assignment
        if (context.mode_stack.top() != Context::Mode::ASSIGN)
        {
            // Dereference
            dst << AST_INDENT << load_ins << " " << dest_reg << ", "
                << "0(" << index_reg << ")" << std::endl;
        }
        else
        {
            Operator::move_reg(dst, index_reg, dest_reg, Types::INT, Types::INT);
        }

        context.deallocate_register(dst, index_reg);
    }

private:
    // postfix_expression '[' expression ']'
    Identifier* identifier;
    Node* index;
    mutable std::string index_reg;
};

#endif // ast_array_access_hpp

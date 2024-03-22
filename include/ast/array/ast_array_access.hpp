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
        Expression* _postfix_expression,
        Expression* _index
    ) :
        postfix_expression(_postfix_expression),
        index(_index)
    {}

    virtual ~ArrayAccess()
    {
        delete postfix_expression;
        delete index;
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        postfix_expression->print(dst, 0);
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
            return postfix_expression->get_type(context);
        }
    }

    std::string get_id() const override
    {
        return postfix_expression->get_id();
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string id = get_id();
        Types type = context.get_type(id);
        std::string load_ins = context.get_load_instruction(type);

        // Index must be an integer
        std::string index_reg = context.allocate_register(
            dst, Types::INT, {dest_reg});

        unsigned int multiplier = 1;

        recurse_gen_asm(dst, index_reg, context, multiplier, 1);
        load_array_address(dst, context, dest_reg, id, type, index_reg);

        // No need to load for assignment
        if (context.mode_stack.top() == Context::Mode::ASSIGN ||
            context.mode_stack.top() == Context::Mode::ADDRESS
        ) {
            Operator::move_reg(dst, index_reg, dest_reg, Types::INT, Types::INT);
        }
        else
        {
            // Dereference
            dst << AST_INDENT << load_ins << " " << dest_reg << ", "
                << "0(" << index_reg << ")" << std::endl;
        }

        context.deallocate_register(dst, index_reg);
    }

    /**
     *  This function works out the index offset of an array, supporting
     *  multi-dimensional arrays.
     *
     *  Depth must start as 1 for this to work correctly. We don't care about
     *  index 0 of the dimension array (in C it can be omitted).
    */
    void recurse_gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context,
        unsigned int &multiplier,
        unsigned int depth
    ) const {
        // Not good Leetcode, whatever bro
        std::string id = get_id();
        auto dimensions = context.get_function_variable(id).array_dimensions;

        // Do we need to recurse?
        ArrayAccess *array_access = dynamic_cast<ArrayAccess*>(postfix_expression);
        if (array_access)
        {
            // Recurse
            array_access->recurse_gen_asm(
                dst, dest_reg, context, multiplier, depth + 1);

            // Coming back up...

            // Depth must start at 1
            multiplier /= dimensions[dimensions.size() - depth];

            std::string index_reg = context.allocate_register(
                dst, Types::INT, {dest_reg});
            index->gen_asm(dst, index_reg, context);

            std::string mul_reg = context.allocate_register(
                dst, Types::INT, {dest_reg});
            dst << AST_INDENT << "li " << mul_reg << ", "
                << multiplier << std::endl;
            dst << AST_INDENT << "mul " << index_reg << ", "
                << index_reg << ", " << mul_reg << std::endl;
            dst << AST_INDENT << "add " << dest_reg << ", "
                << dest_reg << ", " << index_reg << std::endl;

            context.deallocate_register(dst, index_reg);
            context.deallocate_register(dst, mul_reg);
        }
        else
        {
            // Bottom of the tree
            Types type = context.get_type(id);
            multiplier = context.type_size_map.at(type);

            if (dimensions.size() > 1)
            {
                for (unsigned int i = 1; i < dimensions.size(); i++)
                {
                    multiplier *= dimensions[i];
                }
            }

            index->gen_asm(dst, dest_reg, context);

            std::string mul_reg = context.allocate_register(
                dst, Types::INT, {dest_reg});
            dst << AST_INDENT << "li " << mul_reg << ", "
                << multiplier << std::endl;
            dst << AST_INDENT << "mul " << dest_reg << ", "
                << dest_reg << ", " << mul_reg << std::endl;
            context.deallocate_register(dst, mul_reg);
        }
    }

private:

    // Helper function to clean up code
    void load_array_address(
        std::ostream &dst,
        Context &context,
        std::string reg,
        std::string id,
        Types type,
        std::string index_reg
    ) const {
        std::string load_ins = context.get_load_instruction(type);
        std::string lui_reg, base_ptr_reg;
        std::string load_id;
        int base_pointer = context.get_stack_location(id);

        if (context.get_function_variable(id).scope == Scope::GLOBAL)
        {
            lui_reg = context.allocate_register(dst, Types::INT, {reg});

            dst << AST_INDENT << "lui " << lui_reg
                << ", %hi(" << id << ")" << std::endl;
            dst << AST_INDENT << "addi " << lui_reg << ", "
                << lui_reg << ", %lo(" << id << ")" << std::endl;

            // Add the index
            dst << AST_INDENT << "add " << index_reg << ", "
                << index_reg << ", " << lui_reg << std::endl;

            context.deallocate_register(dst, lui_reg);
        }
        else
        {
            // If base pointer is a pointer, derefence the pointer
            if (context.get_function_variable(id).is_pointer)
            {
                base_ptr_reg = context.allocate_register(
                    dst, Types::INT, {index_reg});

                dst << AST_INDENT << "lw " << base_ptr_reg << ", "
                    << base_pointer << "(s0)" << std::endl;
                dst << AST_INDENT << "add " << index_reg << ", "
                    << index_reg << ", " << base_ptr_reg << std::endl;

                context.deallocate_register(dst, base_ptr_reg);
            }
            else
            {
                // Add the base pointer
                dst << AST_INDENT << "addi " << index_reg << ", "
                    << index_reg << ", " << base_pointer << std::endl;
                // Add s0
                dst << AST_INDENT << "add " << index_reg << ", "
                    << index_reg << ", s0" << std::endl;
            }
        }
    }

    // postfix_expression '[' expression ']'
    Expression *postfix_expression;
    Node* index;
};

#endif // ast_array_access_hpp

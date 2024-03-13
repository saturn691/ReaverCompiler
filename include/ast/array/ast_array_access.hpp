#ifndef ast_array_access_hpp
#define ast_array_access_hpp

#include "../ast_node.hpp"
#include "../ast_context.hpp"

#include <cmath>

/*
 *  Node for array access (e.g. "arr[5];")
*/
class ArrayAccess : public Node
{
public:
    ArrayAccess(
        Node* _array,
        Node* _index
    ) :
        array(_array),
        index(_index)
    {}

    virtual ~ArrayAccess()
    {
        delete array;
        delete index;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        array->print(dst, 0);
        dst << "[";
        index->print(dst, 0);
        dst << "]";
    }

    std::string get_index_register() const
    {
        return index_register;
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        Types type = array->get_type(context);
        std::string reg = context.allocate_register(type);
        std::string id = array->get_id();

        index->gen_asm(dst, reg, context);
        index_register = reg;

        int size = context.get_size(id);
        int log_size = log2(size);
        int base_pointer = context.get_stack_location(id);

        /*
            If the array is accessed through pointer element access operator
            "[]", we need to dereference it otherwise, we can just access the
            array directly
        */
        if (context.get_is_pointer(id))
        {
            std::string addr_reg = context.allocate_register(Types::INT);

            // Dereference the pointer to get the base address
            dst << indent << "lw " << addr_reg << ", "
                << base_pointer << "(s0)" << std::endl;
            dst << indent << "slli " << reg << ", " << reg
                << ", " << log_size << std::endl;
            dst << indent << "add " << reg << ", " << reg
                << ", " << addr_reg << std::endl;

            context.deallocate_register(addr_reg);
        }
        else
        {
            dst << indent << "slli " << reg << ", " << reg
                << ", " << log_size << std::endl;
            dst << indent << "addi " << reg << ", " << reg
                << ", " << base_pointer << std::endl;
            dst << indent << "add " << reg << ", " << reg
                << ", s0" << std::endl;
        }

        // Load the value from the array
        std::string load = Context::get_load_instruction(type);
        dst << indent << load << " " << dest_reg
            << ", 0(" << reg << ")" << std::endl;

        // intentionally do not deallocate the register
        // will be deallocated in assign
    }

private:
    // postfix_expression '[' expression ']'
    Node* array;
    Node* index;
    mutable std::string index_register;
};

#endif // ast_array_access_hpp

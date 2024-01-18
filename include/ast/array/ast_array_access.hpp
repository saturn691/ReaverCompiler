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
        NodePtr _array,
        NodePtr _index
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

    virtual std::string get_id() const override
    {
        return array->get_id();
    }

    virtual Types get_type(Context &context) const override
    {
        return array->get_type(context);
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("ArrayAccess::evaluate() not implemented");
    }

    std::string get_index_register() const
    {
        std::cout << "In get_index_register, index_register: " << index_register << std::endl;
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

        dst << indent << "slli " << reg << ", " << reg
            << ", " << log_size << std::endl;

        dst << indent << "addi " << reg << ", " << reg
            << ", " << base_pointer << std::endl;

        dst << indent << "add " << reg << ", " << reg
            << ", s0" << std::endl;

        // dst << "fuck";
        dst << indent << "lw " << dest_reg << ", 0(" << reg << ")" << std::endl;

        // std::string rhs_reg = context.allocate_register(type);
        // int idx = context.get_stack_location(index->get_id());

        // index->gen_asm(dst, rhs_reg, context);
        // dst << indent << "sw " << rhs_reg << ", 0(" << reg << ")" << std::endl;

        // context.deallocate_register(rhs_reg);

        // context.deallocate_register(reg); // <-- I don't know how to prevent deallocation

        // Context::register_map[dest_reg] = idx; // <-- idk
    }

private:
    // postfix_expression '[' expression ']'
    NodePtr array;
    NodePtr index;
    mutable std::string index_register;
};

#endif // ast_array_access_hpp

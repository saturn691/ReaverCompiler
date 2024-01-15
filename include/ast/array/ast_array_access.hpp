#ifndef ast_array_access_hpp
#define ast_array_access_hpp

#include "../ast_node.hpp"
#include "../ast_context.hpp"

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

    virtual Types get_type(Context &context) const override
    {
        throw std::runtime_error("ArratAccess:get_type() not implemented");
        // return Types::Unknown;
    }

    virtual double evaluate(Context &context) const override
    {
        // TODO: Implement evaluation for array access
        throw std::runtime_error("ArrayAccess::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        Types type = array->get_type(context);
        std::string id = array->get_id();
        std::string arr_reg = context.allocate_register(type);

        int idx = index->evaluate(context);
        int offset = context.get_size(id)*idx;
        int stack_loc = context.get_stack_location(id) + offset;

        // For debugging
        // dst << "index: " << idx << std::endl;
        // dst <<  "offset: " << offset << std::endl;
        // dst << "stack_loc: " << stack_loc << std::endl;

        array->gen_asm(dst, arr_reg, context);

        context.deallocate_register(arr_reg);
    }

private:
    // postfix_expression '[' expression ']'
    NodePtr array;
    NodePtr index;
};

#endif // ast_array_access_hpp

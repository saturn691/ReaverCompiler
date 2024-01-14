#ifndef ast_decrement_hpp
#define ast_decrement_hpp

#include "../ast_node.hpp"
#include "../ast_context.hpp"

// Node for post-decrement (e.g., i--)
class PostDecrement : public Node {
public:
    PostDecrement(
        NodePtr _operand
    ) :
        operand(_operand)
    {}

    virtual ~PostDecrement() {
        delete operand;
    }

    virtual void print(std::ostream &dst, int indent_level) const override {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');
        operand->print(dst, indent_level);
        dst << "--" << std::endl;
    }

    virtual Types get_type(Context &context) const override
    {
        return operand->get_type(context);
    }

    virtual unsigned int get_size(Context &context) const override
    {
        return operand->get_size(context);
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        std::string temp_reg = context.allocate_register(get_type(context));

        operand->gen_asm(dst, temp_reg, context); // x
        dst << indent << "addi " << temp_reg
            << ", " << temp_reg << ", -1" << std::endl; // --

        int stack_loc = context.get_stack_location(operand->get_id());
        dst << indent << "sw " << temp_reg
            << ", " << stack_loc << "(s0)" << std::endl;

        context.deallocate_register(temp_reg);
    }

private:
    NodePtr operand;  // The operand to be decremented
};

// Node for pre-decrement (e.g., --i)
class PreDecrement : public Node {
public:
    PreDecrement(
        NodePtr _operand
    ) :
        operand(_operand)
    {}

    virtual ~PreDecrement() {
        delete operand;
    }

    virtual unsigned int get_size(Context &context) const override
    {
        return operand->get_size(context);
    }

    virtual void print(std::ostream &dst, int indent_level) const override {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');
        dst << "--" << std::endl;
        operand->print(dst, indent_level);
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        throw std::runtime_error("PreDecrement::gen_asm() not implemented");
    }

private:
    NodePtr operand;  // The operand to be decremented
};

#endif // ast_decrement_hpp

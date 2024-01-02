#ifndef AST_INCREMENT_HPP
#define AST_INCREMENT_HPP

#include "../ast_node.hpp"

// Node for post-increment (e.g., i++)
class PostIncrement : public Node {
public:
    PostIncrement(
        NodePtr _operand
    ) :
        operand(_operand)
    {}

    virtual ~PostIncrement() {
        delete operand;
    }

    virtual void print(std::ostream &dst, int indent_level) const override {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');
        operand->print(dst, indent_level);
        dst << "++" << std::endl;
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        operand->gen_asm(dst, dest_reg, context);
        dst << indent << "addi " << dest_reg << ", " << dest_reg << ", 1" << std::endl;
        // if (operand->isVariable()) {
        //     dst << "sw " << dest_reg << ", " << stack_offset << "(s0)" << std::endl;
        // }
    }

private:
    NodePtr operand;  // The operand to be incremented
};

// Node for pre-increment (e.g., ++i)
class PreIncrement : public Node {
public:
    PreIncrement(
        NodePtr _operand
    ) :
        operand(_operand)
    {}

    virtual ~PreIncrement() {
        delete operand;
    }

    virtual void print(std::ostream &dst, int indent_level) const override {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');
        dst << "++" << std::endl;
        operand->print(dst, indent_level);
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        throw std::runtime_error("PreIncrement::gen_asm() not implemented");
    }

private:
    NodePtr operand;  // The operand to be incremented
};

#endif // AST_INCREMENT_HPP

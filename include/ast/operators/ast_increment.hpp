#ifndef AST_INCREMENT_HPP
#define AST_INCREMENT_HPP

#include "../ast_node.hpp"
#include "../primitives/ast_number.hpp"

// Node for post-increment (e.g., i++)
class PostIncrement : public Node {
public:
    PostIncrement(
        NodePtr _operand,
        bool _invert = false
    ) :
        operand(_operand),
        invert(_invert)
    {}

    virtual ~PostIncrement() {
        delete operand;
    }

    virtual void print(std::ostream &dst, int indent_level) const override {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');
        operand->print(dst, indent_level);
        dst << "++" << std::endl;
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
        Types type = get_type(context);
        std::string temp_reg = context.allocate_register(type);

        operand->gen_asm(dst, temp_reg, context); // x

        /*
            Essentially:

            1. Evaluate operand
            2. Increment operand
            3. Store incremented value
        */

        std::string store = context.get_store_instruction(type);
        std::string add = (type == Types::FLOAT) ? "fadd.s" : "fadd.d";
        std::string number = (invert ? "-1" : "1");
        std::string one_reg;
        // We can call the gen_asm function of the Number class if it's a float
        Number one_node(1.0 * (invert ? -1 : 1));

        switch (type)
        {
            case Types::FLOAT:
            case Types::DOUBLE:
            case Types::LONG_DOUBLE:
                one_reg = context.allocate_register(Types::FLOAT);
                one_node.gen_asm(dst, one_reg, context);

                dst << indent << add << " " << temp_reg
                    << ", " << temp_reg << ", " << one_reg << std::endl; // ++
                break;

            default:
                dst << indent << "addi " << temp_reg
                    << ", " << temp_reg << ", " << number << std::endl; // ++
        }

        int stack_loc = context.get_stack_location(operand->get_id());
        dst << indent << store << " " << temp_reg
            << ", " << stack_loc << "(s0)" << std::endl;

        context.deallocate_register(temp_reg);
    }

private:
    NodePtr operand;  // The operand to be incremented
    bool invert;  // Switch between increment and decrement
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

    virtual unsigned int get_size(Context &context) const override
    {
        return operand->get_size(context);
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

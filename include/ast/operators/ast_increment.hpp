#ifndef AST_INCREMENT_HPP
#define AST_INCREMENT_HPP

#include "../ast_node.hpp"
#include "../ast_expression.hpp"
#include "../primitives/ast_number.hpp"

// Node for post-increment (e.g., i++)
class PostIncrement : public Expression {
public:
    PostIncrement(
        Expression* _operand,
        bool _invert = false,
        bool _pre = false
    ) :
        operand(_operand),
        invert(_invert),
        pre(_pre)
    {}

    virtual ~PostIncrement() {
        delete operand;
    }

    void print(std::ostream &dst, int indent_level) const override {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');
        std::string op = (invert ? "--" : "++");

        if (pre)
        {
            dst << AST_INDENT << op;
            operand->print(dst, 0);
        }
        else
        {
            operand->print(dst, indent_level);
            dst << op;
        }
    }

    Types get_type(Context &context) const override
    {
        return operand->get_type(context);
    }

    std::string get_id() const override
    {
        return operand->get_id();
    }

    double evaluate() const override {
        double value = operand->evaluate();
        // If pre-increment, return the incremented value
        // If invert, decrement instead of increment
        return (pre ? (invert ? value - 1 : value + 1) : value);
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        /*
            Essentially:

            1. Evaluate operand
            2. Increment operand
            3. Store incremented value (to memory)
            4. Dest reg = original value (if post-increment) for returns

            Code may be a bit unreadable but just follow this in order.
        */

        Types type = get_type(context);
        int invert_val = invert ? -1 : 1;

        Identifier* identifier = dynamic_cast<Identifier*>(operand);
        if (identifier)
        {
            FunctionVariable fv = context
                .get_function_variable(identifier->get_id());

            if (fv.is_pointer)
            {
                invert_val *= context.type_size_map.at(fv.type);
            }
        }

        std::string temp_reg = context.allocate_register(dst, type, {dest_reg});
        Context::Mode mode = context.mode_stack.top();
        context.mode_stack.push(Context::Mode::OPERATOR);

        operand->gen_asm(dst, temp_reg, context); // x

        // We need to store the original value if this is a post-increment
        if (!pre && mode == Context::Mode::RETURN)
        {
            std::string move_ins = move_ins_map.at(type);
            dst << AST_INDENT << move_ins << " " << dest_reg
                << ", " << temp_reg << std::endl;
        }

        std::string add = (type == Types::FLOAT) ? "fadd.s" : "fadd.d";
        std::string number = std::to_string(invert_val);
        if (type == Types::FLOAT)
        {
            number += "f";
        }

        std::string one_reg;
        // We can call the gen_asm function of the Number class if it's a float
        Number one_node(number);

        switch (type)
        {
            case Types::FLOAT:
            case Types::DOUBLE:
            case Types::LONG_DOUBLE:
                one_reg = context.allocate_register(
                    dst, Types::FLOAT, {dest_reg, temp_reg});
                one_node.gen_asm(dst, one_reg, context);

                dst << AST_INDENT << add << " " << temp_reg
                    << ", " << temp_reg << ", " << one_reg << std::endl; // ++
                break;

            default:
                dst << AST_INDENT << "addi " << temp_reg
                    << ", " << temp_reg << ", " << number << std::endl; // ++
        }

        context.store(dst, temp_reg, operand->get_id(), type);

        // Pre-increment means we store the incremented value
        if (pre && mode == Context::Mode::RETURN)
        {
            std::string move_ins = move_ins_map.at(type);
            dst << AST_INDENT << move_ins << " " << dest_reg
                << ", " << temp_reg << std::endl;
        }

        context.mode_stack.pop();
        context.deallocate_register(dst, temp_reg);
    }

private:
    Expression* operand;  // The operand to be incremented
    bool invert;  // Switch between increment and decrement
    bool pre;  // Whether this is a pre-increment or post-increment

    const std::unordered_map<Types, std::string> move_ins_map = {
        {Types::UNSIGNED_CHAR, "mv"},
        {Types::CHAR, "mv"},
        {Types::UNSIGNED_SHORT, "mv"},
        {Types::SHORT, "mv"},
        {Types::UNSIGNED_INT, "mv"},
        {Types::INT, "mv"},
        {Types::UNSIGNED_LONG, "mv"},
        {Types::LONG, "mv"},
        {Types::FLOAT, "fmv.s"},
        {Types::DOUBLE, "fmv.d"},
        {Types::LONG_DOUBLE, "fmv.d"}
    };

};


#endif // AST_INCREMENT_HPP

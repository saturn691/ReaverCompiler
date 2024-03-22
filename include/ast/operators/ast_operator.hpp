#ifndef AST_OPERATOR_HPP
#define AST_OPERATOR_HPP

#include "../ast_node.hpp"
#include <cmath>


enum class OperatorType
{
    ADD,
    SUB,
    MOD,
    MUL,
    DIV,
    BITWISE_AND,
    BITWISE_OR,
    BITWISE_XOR,
    LEFT_SHIFT,
    RIGHT_SHIFT
};

/*
 *  Base class for all operators (e.g. + - * / << >> & |)
*/
class Operator : public Expression
{
public:
    Operator(Expression* _left, Expression* _right) :
        left(_left),
        right(_right)
    {}

    Operator(
        Expression* _left,
        Expression* _right,
        OperatorType _otype
    ) :
        left(_left),
        right(_right),
        otype(_otype)
    {}

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES * indent_level), ' ');

        dst << indent;
        get_left()->print(dst, indent_level);
        dst << " " << symbol_map.at(otype) << " ";
        get_right()->print(dst, indent_level);
        dst << std::endl;
    }

    std::string get_id() const override
    {
        // Does not exist for operators.
        throw std::runtime_error("Operator::get_id() not implemented");
    }

    Types get_type(Context &context) const override
    {
        return std::max(left->get_type(context), right->get_type(context));
    }

    Expression* get_left() const
    {
        return left;
    }

    Expression* get_right() const
    {
        return right;
    }

    double evaluate() const override
    {
        double l = left->evaluate();
        double r = right->evaluate();

        switch (otype)
        {
            case OperatorType::ADD:
                return l + r;
            case OperatorType::SUB:
                return l - r;
            case OperatorType::MOD:
                return fmod(l, r);
            case OperatorType::MUL:
                return l * r;
            case OperatorType::DIV:
                return l / r;
            case OperatorType::BITWISE_AND:
                return (int)l & (int)r;
            case OperatorType::BITWISE_OR:
                return (int)l | (int)r;
            case OperatorType::BITWISE_XOR:
                return (int)l ^ (int)r;
            case OperatorType::LEFT_SHIFT:
                return (int)l << (int)r;
            case OperatorType::RIGHT_SHIFT:
                return (int)l >> (int)r;
            default:
                throw std::runtime_error("Unknown operator type");
        }
    }

    /**
     *  Section 6.5.6 of the ISO C90 standard, paragraphs 8 and 9:
     *
     *  Essentially pointer arithmetic is allowed, but only if the pointer
     *  arithmetic is done on an array.
     *
     *  Pointer arithmetic is only defined for subtracted between two array
     *  pointers if they point to the same array.
    */
    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override
    {
        Types type = std::max(get_left()->get_type(context),
                              get_right()->get_type(context));
        context.mode_stack.push(Context::Mode::OPERATOR);

        get_left()->gen_asm(dst, dest_reg, context);

        std::string temp_reg1 = context.allocate_register(dst, type, {dest_reg});
        get_right()->gen_asm(dst, temp_reg1, context);

        // Pointer arithmetic is only defined for add and sub (else, UB)
        if (otype == OperatorType::ADD || otype == OperatorType::SUB)
        {
            // Check if the left is a pointer
            try
            {
                std::string l_id = get_left()->get_id();
                FunctionVariable fv = context.get_function_variable(l_id);
                if (fv.is_pointer && !fv.is_array)
                {
                    // Get the size of the type
                    unsigned int size = context.type_size_map.at(fv.type);
                    unsigned int log_size = log2(size);

                    // Multiply the right by the size
                    dst << AST_INDENT << "slli " << temp_reg1 << ", "
                        << temp_reg1 << ", " << log_size << std::endl;
                }
            }
            catch (...) {}

            // Check the right is a pointer
            try
            {
                std::string r_id = get_right()->get_id();
                FunctionVariable fv = context.get_function_variable(r_id);
                if (fv.is_pointer && !fv.is_array)
                {
                    // Get the size of the type
                    unsigned int size = context.type_size_map.at(fv.type);
                    unsigned int log_size = log2(size);

                    // Multiply the left by the size
                    dst << AST_INDENT << "slli " << dest_reg << ", "
                        << dest_reg << ", " << log_size << std::endl;
                }
            }
            catch (...) {}
        }

        dst << AST_INDENT << ins_map.at(otype).at(type) << " " << dest_reg
            << ", " << dest_reg << ", " << temp_reg1 << std::endl;

        context.deallocate_register(dst, temp_reg1);
        context.mode_stack.pop();
    }

    /**
     * Generate RISC-V assembly to move a value from one register to another.
     * Takes into account the type of the source register and the destination,
     * and performs the necessary conversion if required.
    */
    static void move_reg(
        std::ostream &dst,
        std::string src_reg,
        std::string dest_reg,
        Types src_type,
        Types dest_type
    ) {
        // Redundant move
        if (src_reg == dest_reg)
        {
            return;
        }

        // Floating move with conversion to INT
        std::string fmv_conv_int;
        fmv_conv_int = (src_type == Types::FLOAT) ? "fcvt.w.s" : "fcvt.w.d";

        // Floating move with conversion to FLOAT
        std::string fmv_conv_float;
        fmv_conv_float = (dest_type == Types::FLOAT) ? "fcvt.s.w" : "fcvt.d.w";

        // Floating move FLOAT TO FLOAT
        std::string fmv;
        if (src_type == Types::FLOAT)
        {
            fmv = (dest_type == Types::FLOAT) ? "fmv.s" : "fcvt.d.s";
        }
        else
        {
            fmv = (dest_type == Types::FLOAT) ? "fcvt.s.d" : "fmv.d";
        }


        switch (src_type)
        {
            case Types::FLOAT:
            case Types::DOUBLE:
            case Types::LONG_DOUBLE:
                if (dest_reg[0] != 'f')
                {
                    dst << AST_INDENT << fmv_conv_int << " " << dest_reg
                        << ", " << src_reg << ", rtz"<< std::endl;
                }
                else
                {
                    dst << AST_INDENT << fmv << " " << dest_reg
                        << ", " << src_reg << std::endl;
                }
                break;

            default:
                if (dest_reg[0] == 'f')
                {
                    dst << AST_INDENT << fmv_conv_float << " " << dest_reg
                        << ", " << src_reg << std::endl;
                }
                else
                {
                    dst << AST_INDENT << "mv " << dest_reg
                        << ", " << src_reg << std::endl;
                }
        }

        return;
    }

    /**
     *  Helper functon to determine if a move is needed
    */
    static bool reg_type_match(std::string reg, Types type)
    {
        if (reg[0] == 'f')
        {
            return (type == Types::FLOAT ||
                    type == Types::DOUBLE ||
                    type == Types::LONG_DOUBLE);
        }
        else
        {
            return (type != Types::FLOAT &&
                    type != Types::DOUBLE &&
                    type != Types::LONG_DOUBLE);
        }
    }

private:

    Expression* left;
    Expression* right;
    OperatorType otype;

    const std::unordered_map<OperatorType, std::string> symbol_map = {
        {OperatorType::ADD, "+"},
        {OperatorType::SUB, "-"},
        {OperatorType::MOD, "%"},
        {OperatorType::MUL, "*"},
        {OperatorType::DIV, "/"},
        {OperatorType::BITWISE_AND, "&"},
        {OperatorType::BITWISE_OR, "|"},
        {OperatorType::BITWISE_XOR, "^"},
        {OperatorType::LEFT_SHIFT, "<<"},
        {OperatorType::RIGHT_SHIFT, ">>"},
    };

    const std::unordered_map<OperatorType, std::unordered_map<Types, std::string>> ins_map = {
        {OperatorType::ADD, {
            {Types::UNSIGNED_CHAR, "add"},
            {Types::CHAR, "add"},
            {Types::UNSIGNED_SHORT, "add"},
            {Types::SHORT, "add"},
            {Types::INT, "add"},
            {Types::UNSIGNED_INT, "add"},
            {Types::UNSIGNED_LONG, "add"},
            {Types::LONG, "add"},
            {Types::FLOAT, "fadd.s"},
            {Types::DOUBLE, "fadd.d"},
            {Types::LONG_DOUBLE, "fadd.d"}
        }},
        {OperatorType::SUB, {
            {Types::UNSIGNED_CHAR, "sub"},
            {Types::CHAR, "sub"},
            {Types::UNSIGNED_SHORT, "sub"},
            {Types::SHORT, "sub"},
            {Types::INT, "sub"},
            {Types::UNSIGNED_INT, "sub"},
            {Types::UNSIGNED_LONG, "sub"},
            {Types::LONG, "sub"},
            {Types::FLOAT, "fsub.s"},
            {Types::DOUBLE, "fsub.d"},
            {Types::LONG_DOUBLE, "fsub.d"}
        }},
        {OperatorType::MOD, {
            {Types::UNSIGNED_CHAR, "remu"},
            {Types::CHAR, "rem"},
            {Types::UNSIGNED_SHORT, "remu"},
            {Types::SHORT, "rem"},
            {Types::INT, "rem"},
            {Types::UNSIGNED_INT, "remu"},
            {Types::LONG, "rem"},
            {Types::UNSIGNED_LONG, "remu"},
        }},
        {OperatorType::MUL, {
            {Types::UNSIGNED_CHAR, "mul"},
            {Types::CHAR, "mul"},
            {Types::UNSIGNED_SHORT, "mul"},
            {Types::SHORT, "mul"},
            {Types::INT, "mul"},
            {Types::UNSIGNED_INT, "mul"},
            {Types::UNSIGNED_LONG, "mul"},
            {Types::LONG, "mul"},
            {Types::FLOAT, "fmul.s"},
            {Types::DOUBLE, "fmul.d"},
            {Types::LONG_DOUBLE, "fmul.d"}
        }},
        {OperatorType::DIV, {
            {Types::UNSIGNED_CHAR, "div"},
            {Types::CHAR, "div"},
            {Types::UNSIGNED_SHORT, "div"},
            {Types::SHORT, "div"},
            {Types::INT, "div"},
            {Types::UNSIGNED_INT, "div"},
            {Types::UNSIGNED_LONG, "div"},
            {Types::LONG, "div"},
            {Types::FLOAT, "fdiv.s"},
            {Types::DOUBLE, "fdiv.d"},
            {Types::LONG_DOUBLE, "fdiv.d"}
        }},
        {OperatorType::BITWISE_AND, {
            {Types::UNSIGNED_CHAR, "and"},
            {Types::CHAR, "and"},
            {Types::UNSIGNED_SHORT, "and"},
            {Types::SHORT, "and"},
            {Types::INT, "and"},
            {Types::UNSIGNED_INT, "and"},
            {Types::UNSIGNED_LONG, "and"},
            {Types::LONG, "and"},
        }},
        {OperatorType::BITWISE_OR, {
            {Types::UNSIGNED_CHAR, "or"},
            {Types::CHAR, "or"},
            {Types::UNSIGNED_SHORT, "or"},
            {Types::SHORT, "or"},
            {Types::INT, "or"},
            {Types::UNSIGNED_INT, "or"},
            {Types::UNSIGNED_LONG, "or"},
            {Types::LONG, "or"},
        }},
        {OperatorType::BITWISE_XOR, {
            {Types::UNSIGNED_CHAR, "xor"},
            {Types::CHAR, "xor"},
            {Types::UNSIGNED_SHORT, "xor"},
            {Types::SHORT, "xor"},
            {Types::INT, "xor"},
            {Types::UNSIGNED_INT, "xor"},
            {Types::UNSIGNED_LONG, "xor"},
            {Types::LONG, "xor"},
        }},
        {OperatorType::LEFT_SHIFT, {
            {Types::UNSIGNED_CHAR, "sll"},
            {Types::CHAR, "sll"},
            {Types::UNSIGNED_SHORT, "sll"},
            {Types::SHORT, "sll"},
            {Types::INT, "sll"},
            {Types::UNSIGNED_INT, "sll"},
            {Types::UNSIGNED_LONG, "sll"},
            {Types::LONG, "sll"},
        }},
        {OperatorType::RIGHT_SHIFT, {
            {Types::UNSIGNED_CHAR, "srl"},
            {Types::CHAR, "sra"},
            {Types::UNSIGNED_SHORT, "srl"},
            {Types::SHORT, "sra"},
            {Types::INT, "sra"},
            {Types::UNSIGNED_INT, "srl"},
            {Types::UNSIGNED_LONG, "srl"},
            {Types::LONG, "sra"},
        }}
    };
};


#endif /* AST_OPERATOR_HPP */

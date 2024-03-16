#ifndef ast_operator_hpp
#define ast_operator_hpp

#include "../ast_node.hpp"


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

    void print(std::ostream &dst, int indent_level) const override
    {
        throw std::runtime_error("Operator::print() not implemented");
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

    void gen_ins(
        std::ostream &dst,
        Types type,
        std::string temp_reg1,
        std::string temp_reg2,
        std::string dest_reg,
        const std::unordered_map<Types, std::string> ins_map,
        bool no_convert = false
    ) const {
        // Don't question no_convert @booth-algo, it's a hack
        // Essentially some comparison operators work slightly differently
        std::string float_move_ins = (type == Types::FLOAT) ? "fcvt.w.s" : "fcvt.w.d";

        switch (type)
        {
            case Types::FLOAT:
            case Types::DOUBLE:
            case Types::LONG_DOUBLE:
                if (dest_reg[0] != 'f' && !no_convert)
                {
                    /*
                        1. Put into floating point register
                        2. Move to dest register
                        3. rtz = round to zero
                    */

                    dst << AST_INDENT << ins_map.at(type) << " " << temp_reg1
                        << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;

                    dst << AST_INDENT << float_move_ins << " " << dest_reg
                        << ", " << temp_reg1 << ", rtz"<< std::endl;
                }
                else
                {
                    dst << AST_INDENT << ins_map.at(type) << " " << dest_reg
                        << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;
                }
                break;

            default:
                dst << AST_INDENT << ins_map.at(type) << " " << dest_reg
                    << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;
        }
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

private:
    Expression* left;
    Expression* right;
};


#endif /* ast_operator_hpp */

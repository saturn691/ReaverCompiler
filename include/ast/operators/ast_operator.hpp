#ifndef ast_operator_hpp
#define ast_operator_hpp

#include "../ast_node.hpp"


/*
 *  Base class for all operators (e.g. + - * / << >> & |)
*/
class Operator : public BinaryNode
{
public:
    using BinaryNode::BinaryNode;

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        throw std::runtime_error("Operator::print() not implemented");
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

        std::string indent(AST_PRINT_INDENT_SPACES, ' ');

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

                    dst << indent << ins_map.at(type) << " " << temp_reg1
                        << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;

                    dst << indent << float_move_ins << " " << dest_reg
                        << ", " << temp_reg1 << ", rtz"<< std::endl;
                }
                else
                {
                    dst << indent << ins_map.at(type) << " " << dest_reg
                        << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;
                }
                break;

            default:
                dst << indent << ins_map.at(type) << " " << dest_reg
                    << ", " << temp_reg1 << ", " << temp_reg2 << std::endl;
        }
    }
};


#endif /* ast_operator_hpp */

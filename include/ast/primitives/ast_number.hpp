#ifndef ast_number_hpp
#define ast_number_hpp

#include "../ast_node.hpp"
#include <cstring>


/*
 *  Leaf node for numbers (e.g. "10" in "int x = 10;")
*/
class Number : public Node
{
public:
    Number(double _value) : value(_value) {}

    virtual ~Number()
    {}

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        dst << value;
    }

    virtual Types get_type(Context &context) const override
    {
        // Here return the lowest priority type
        return Types::VOID;
    }

    virtual double evaluate(Context &context) const override
    {
        return value;
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');

        /*
        GCC compiles the following

        ; float acc=1.0f;
            fsw	fa0,-36(s0)
            sw	a0,-40(s0)
            lui	a5,%hi(.LC0)

        Therefore we need to allocate a temporary register to hold the upper
        immediate. We're not going to create a label for cleanliness.
        */
        if (dest_reg[0] == 'f')
        {
            float float_value = (float)value;
            uint32_t ieee754_value;
            std::memcpy(&ieee754_value, &float_value, sizeof(uint32_t));

            std::string label = context.get_unique_label("constant");
            context.add_memory_data(label, ieee754_value);
            std::string lui_reg = context.allocate_register(Types::INT);

            // Accessing main memory to fetch constant value
            dst << indent << "lui " << lui_reg
                << ", %hi(." << label << ")" << std::endl;

            dst << indent << "flw " << dest_reg
                << ", %lo(." << label << ")"
                << "(" << lui_reg << ")" << std::endl;

            context.deallocate_register(lui_reg);
        }
        else
        {
            dst << indent << "li " << dest_reg << ", " << value << std::endl;
        }
    }

private:
    double value;
};


#endif  /* ast_number_hpp */
